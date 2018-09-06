#include "common.h"
#include "vxl.h"

#include <random>
#include <chrono> 
#include <unordered_set>

#include "fmt/printf.h"

namespace ace {
    namespace {
        void write_color(std::vector<uint8_t> &vec, uint32_t color) {
            //    char *val = reinterpret_cast<char *>(&item);
            vec.push_back(static_cast<uint8_t>(color >> 0));
            vec.push_back(static_cast<uint8_t>(color >> 8));
            vec.push_back(static_cast<uint8_t>(color >> 16));
            vec.push_back(static_cast<uint8_t>(0x7F)); // ignore lighting byte; I use it to store block health.
        }

        // adapted from VOXLAP5.C by Ken Silverman <http://advsys.net/ken/>
        uint32_t gkrand = 0;
        uint32_t colorjit(uint32_t i, uint32_t jitamount=0x00'07'07'07) {
            gkrand = gkrand * 27584621 + 1;
            return (gkrand & jitamount) ^ i;
        }

        // thanks br[azil]
        int dirt_color_table[] = {
            0x506050, 0x605848, 0x705040,
            0x804838, 0x704030, 0x603828,
            0x503020, 0x402818, 0x302010
        };
        int dirtcolor(int x, int y, int z) {
            int vertical_slice = z >> 3;
            int dirt_base_color = dirt_color_table[vertical_slice];
            int color = (uint16_t)((dirt_base_color
                    + ((z & 7)
                        * ((dirt_color_table[vertical_slice + 1] & 0xFF00)
                            - (dirt_color_table[vertical_slice] & 0xFF00)) >> 3)) & 0xFF00)
                + ((dirt_base_color
                    + ((z & 7) * ((dirt_color_table[vertical_slice + 1] & 0xFF00FF) - (dirt_base_color & 0xFF00FF)) >> 3)) & 0xFF00FF)
                + 4 * (abs((z & 7) - 4) + ((abs((y & 7) - 4) + (abs((x & 7) - 4) << 8)) << 8));
            return (color + 0x10101 * (rand() & 7)) | 0x7F000000;
        }
    }

    glm::u8vec3 jit_color(glm::u8vec3 color) {
        uint32_t col = colorjit(pack_bytes(0xFF, color.r, color.g, color.b));
        return glm::u8vec3(unpack_argb(col));
    }

    AceMap::AceMap(uint8_t *buf) {
        this->read(buf);
    }

    void AceMap::read(uint8_t *buf) {
        fmt::print("READING MAP\n");
        if (!buf) return;

        auto start = std::chrono::high_resolution_clock::now();
        this->geometry.set();
        this->colors.clear();
        this->colors.reserve(MAP_X * MAP_Y * 16); // just a guess tbh

        for (int y = 0; y < MAP_Y; ++y) {
            for (int x = 0; x < MAP_X; ++x) {
                int z = 0;
                while (true) {
                    int number_4byte_chunks = buf[0];
                    int top_color_start = buf[1];
                    int top_color_end = buf[2]; // inclusive

                    for (int i = z; i < top_color_start; i++)
                        this->geometry[get_pos(x, y, i)] = false;

                    uint32_t *color = reinterpret_cast<uint32_t *>(&buf[4]);
                    for (z = top_color_start; z <= top_color_end; z++) {
                        this->colors[get_pos(x, y, z)] = (0x7F << 24) | (*color++ & 0x00FFFFFF);
                    }

                    int len_bottom = top_color_end - top_color_start + 1;

                    // check for end of data marker
                    if (number_4byte_chunks == 0) {
                        // infer ACTUAL number of 4-byte chunks from the length of the color data
                        buf += 4 * (len_bottom + 1);
                        break;
                    }

                    // infer the number of bottom colors in next span from chunk length
                    int len_top = (number_4byte_chunks - 1) - len_bottom;

                    // now skip the v pointer past the data to the beginning of the next span
                    buf += buf[0] * 4;

                    int bottom_color_end = buf[3]; // aka air start
                    int bottom_color_start = bottom_color_end - len_top;

                    for (z = bottom_color_start; z < bottom_color_end; ++z) {
                        this->colors[get_pos(x, y, z)] = (0x7F << 24) | (*color++ & 0x00FFFFFF);
                    }
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        fmt::print("MAP READ TIME: {}\n", std::chrono::duration<double>(end - start).count());
    }

    std::vector<uint8_t> AceMap::write() {
        std::vector<uint8_t> v;
        v.reserve(3 * 1024 * 1024);
        int x = 0, y = 0;
        this->write(v, &x, &y);
        return v;
    }

    size_t AceMap::write(std::vector<uint8_t> &v, int *sx, int *sy, int columns) {
        const size_t initial_size = v.size();
        int column = 0;
        const bool all = columns < 0;

        int y, x;
        for (y = *sy; y < MAP_Y; ++y) {
            for (x = *sx; x < MAP_X; ++x) {
                if (!all && column >= columns) {
                    goto done;
                }
                int z = 0;
                while (z < MAP_Z) {
                    // find the air region
                    int air_start = z;
                    while (z < MAP_Z && !this->is_solid_unchecked(get_pos(x, y, z)))
                        ++z;

                    // find the top region
                    int top_colors_start = z;
                    while (z < MAP_Z && this->is_surface(x, y, z))
                        ++z;
                    int top_colors_end = z;

                    // now skip past the solid voxels
                    while (z < MAP_Z && this->is_solid_unchecked(get_pos(x, y, z)) && !this->is_surface(x, y, z))
                        ++z;

                    // at the end of the solid voxels, we have colored voxels.
                    // in the "normal" case they're bottom colors; but it's
                    // possible to have air-color-solid-color-solid-color-air,
                    // which we encode as air-color-solid-0, 0-color-solid-air

                    // so figure out if we have any bottom colors at this point
                    int bottom_colors_start = z;

                    int i = z;
                    while (i < MAP_Z && this->is_surface(x, y, i))
                        ++i;

                    if (i != MAP_Z) {
                        // these are real bottom colors so we can write them
                        while (is_surface(x, y, z))
                            ++z;
                    }
                    int bottom_colors_end = z;

                    // now we're ready to write a span
                    int top_colors_len = top_colors_end - top_colors_start;
                    int bottom_colors_len = bottom_colors_end - bottom_colors_start;

                    int colors = top_colors_len + bottom_colors_len;

                    if (z == MAP_Z)
                        v.push_back(0);
                    else
                        v.push_back(colors + 1);

                    v.push_back(top_colors_start);
                    v.push_back(top_colors_end - 1);
                    v.push_back(air_start);

                    for (i = 0; i < top_colors_len; ++i)
                        write_color(v, this->get_color(x, y, top_colors_start + i));

                    for (i = 0; i < bottom_colors_len; ++i)
                        write_color(v, this->get_color(x, y, bottom_colors_start + i));
                }
                column++;
            }
            *sx = 0;
        }
    done:
        *sx = x;
        *sy = y;
        return v.size() - initial_size;
    }

    bool AceMap::set_block(int x, int y, int z, bool solid, uint32_t color, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        if (!is_valid_pos(pos)) return false;

        this->set_solid_unchecked(pos, solid);
        if (solid)
            this->set_color_unchecked(pos, color);
        else
            this->colors.erase(pos); // TODO wrap this in something

        this->notify_listeners(x, y, z);

        return true;
    }

    bool AceMap::get_block(int x, int y, int z, uint32_t *color, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        if (!is_valid_pos(pos)) return false;

        *color = this->get_color_unchecked(pos, x, y, z);
        return this->is_solid_unchecked(pos);
    }

    bool AceMap::set_solid(int x, int y, int z, bool solid, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        
        if(!is_valid_pos(pos) || !(this->set_solid_unchecked(pos, solid), true)) {
            return false;
        }
        this->notify_listeners(x, y, z);
        return true;
    }

    bool AceMap::is_solid(int x, int y, int z, bool wrapped) const {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        return is_valid_pos(pos) && this->is_solid_unchecked(pos);
    }

    void AceMap::set_color(int x, int y, int z, uint32_t color, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        if (!is_valid_pos(pos) || !this->is_solid_unchecked(pos)) return;

        this->set_color_unchecked(pos, color);

        this->notify_listeners(x, y, z);
    }

    uint32_t AceMap::get_color(int x, int y, int z, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        auto pos = get_pos(x, y, z);
        if (!is_valid_pos(pos)) return 0;

        return this->get_color_unchecked(pos, x, y, z);
    }

    int AceMap::get_z(int x, int y, int start, bool wrapped) {
        if (wrapped) {
            x &= MAP_X - 1;
            y &= MAP_Y - 1;
        }

        for (int z = start; z < MAP_Z; z++) {
            if (this->is_solid(x, y, z)) return z;
        }
        return MAP_Z;
    }

    Face AceMap::hitscan(const glm::dvec3 &p, const glm::dvec3 &d, glm::ivec3 *h) const {
        constexpr size_t VSID = MAP_X;

        h->x = p.x;
        h->y = p.y;
        h->z = p.z;
        if (!is_valid_pos(h->x, h->y, h->z) && (h->z > MAP_Z + 3 || h->z < -3)) {
            return Face::INVALID;
        }

        // ok after 10 minutes of experimenting it seems that this code is equivalent to
        // ixi = d.x < 0.0 ? -1 : 1; (but way faster)
        // thanks ken
        int ixi = reinterpret_cast<const int32_t *>(&d.x)[1] >> 31 | 1;
        int iyi = reinterpret_cast<const int32_t *>(&d.y)[1] >> 31 | 1;
        int izi = reinterpret_cast<const int32_t *>(&d.z)[1] >> 31 | 1;

        float f = 0x3fffffff / VSID;
        float kx, ky, kz;
        if ((fabs(d.x) >= fabs(d.y)) && (fabs(d.x) >= fabs(d.z))) {
            kx = 1024.0;
            if (d.y == 0) ky = f; else ky = std::min(fabs(d.x / d.y) * 1024.0, double(f));
            if (d.z == 0) kz = f; else kz = std::min(fabs(d.x / d.z) * 1024.0, double(f));
        } else if (fabs(d.y) >= fabs(d.z)) {
            ky = 1024.0;
            if (d.x == 0) kx = f; else kx = std::min(fabs(d.y / d.x) * 1024.0, double(f));
            if (d.z == 0) kz = f; else kz = std::min(fabs(d.y / d.z) * 1024.0, double(f));
        } else {
            kz = 1024.0;
            if (d.x == 0) kx = f; else kx = std::min(fabs(d.z / d.x) * 1024.0, double(f));
            if (d.y == 0) ky = f; else ky = std::min(fabs(d.z / d.y) * 1024.0, double(f));
        }
        int dxi = kx; int dx = (p.x - float(h->x)) * kx; if (ixi >= 0) dx = dxi - dx;
        int dyi = ky; int dy = (p.y - float(h->y)) * ky; if (iyi >= 0) dy = dyi - dy;
        int dzi = kz; int dz = (p.z - float(h->z)) * kz; if (izi >= 0) dz = dzi - dz;

        while (true) {
            Face dir;
            if ((dz <= dx) && (dz <= dy)) {
                h->z += izi; dz += dzi; dir = Face(5 - (izi>0));
            } else {
                if (dx < dy) {
                    h->x += ixi; dx += dxi; dir = Face(1 - (ixi>0));
                } else {
                    h->y += iyi; dy += dyi; dir = Face(3 - (iyi>0));
                }
            }
            if (!is_valid_pos(h->x, h->y, h->z) && (h->z > MAP_Z + 3 || h->z < -3)) return Face::INVALID;
            if (this->is_solid(h->x, h->y, h->z)) return dir;
        }
    }

    uint32_t AceMap::get_color_unchecked(size_t pos, int x, int y, int z) {
        auto color = this->colors.find(pos);
        if (color == this->colors.end()) {
            return this->colors[pos] = dirtcolor(x, y, z);
        }
        return color->second;
    }

    std::vector<glm::ivec3> block_line(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2) {
        std::vector<glm::ivec3> ret;
    
        glm::ivec3 c{ x1, y1, z1 };
        glm::ivec3 d{ x2 - x1, y2 - y1, z2 - z1 };
        long ixi, iyi, izi, dx, dy, dz, dxi, dyi, dzi;
        const size_t VSID = MAP_X;
    
        if (d.x < 0) ixi = -1;
        else ixi = 1;
        if (d.y < 0) iyi = -1;
        else iyi = 1;
        if (d.z < 0) izi = -1;
        else izi = 1;
    
        if ((abs(d.x) >= abs(d.y)) && (abs(d.x) >= abs(d.z)))
        {
            dxi = 1024; dx = 512;
            dyi = static_cast<long>(!d.y ? 0x3fffffff / VSID : abs(d.x * 1024 / d.y));
            dy = dyi / 2;
            dzi = static_cast<long>(!d.z ? 0x3fffffff / VSID : abs(d.x * 1024 / d.z));
            dz = dzi / 2;
        }
        else if (abs(d.y) >= abs(d.z))
        {
            dyi = 1024; dy = 512;
            dxi = static_cast<long>(!d.x ? 0x3fffffff / VSID : abs(d.y * 1024 / d.x));
            dx = dxi / 2;
            dzi = static_cast<long>(!d.z ? 0x3fffffff / VSID : abs(d.y * 1024 / d.z));
            dz = dzi / 2;
        }
        else
        {
            dzi = 1024; dz = 512;
            dxi = static_cast<long>(!d.x ? 0x3fffffff / VSID : abs(d.z * 1024 / d.x));
            dx = dxi / 2;
            dyi = static_cast<long>(!d.y ? 0x3fffffff / VSID : abs(d.z * 1024 / d.y));
            dy = dyi / 2;
        }
        if (ixi >= 0) dx = dxi - dx;
        if (iyi >= 0) dy = dyi - dy;
        if (izi >= 0) dz = dzi - dz;
    
        while (true) {
            ret.push_back(c);
    
            if (c.x == x2 &&
                c.y == y2 &&
                c.z == z2)
                break;
    
            if ((dz <= dx) && (dz <= dy))
            {
                c.z += izi;
                if (c.z < 0 || c.z >= MAP_Z)
                    break;
                dz += dzi;
            }
            else
            {
                if (dx < dy)
                {
                    c.x += ixi;
                    if (static_cast<unsigned long>(c.x) >= VSID)
                        break;
                    dx += dxi;
                }
                else
                {
                    c.y += iyi;
                    if (static_cast<unsigned long>(c.y) >= VSID)
                        break;
                    dy += dyi;
                }
            }
        }
        return ret;
    }

    // void AceMap::get_random_point(int *x, int *y, int *z, const int x1, const int y1, const int x2, const int y2) const {
    //     int rx = 0, ry = 0, rz = 0;
    //     for (int i = 0; i < 16; i++) {
    //         rx = ace::random::random(x1, x2 - 1); ry = ace::random::random(y1, y2 - 1); rz = this->get_z(rx, ry);
    //         if (rz < MAP_Z - 2 && this->get_solid(rx, ry, rz)) {
    //             break;
    //         }
    //     }
    //     *x = rx; *y = ry; *z = rz;
    // }
    //
    // glm::ivec3 AceMap::get_random_point(glm::ivec2 p1, glm::ivec2 p2) const {
    //     glm::ivec3 p;
    //     this->get_random_point(&p.x, &p.y, &p.z, p1.x, p1.y, p2.x, p2.y);
    //     return p;
    // }
    //
   
}
