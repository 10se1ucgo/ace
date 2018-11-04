#include "draw/map.h"

#include "game_client.h"
#include "draw/sprite.h"
#include "scene/game.h"

namespace ace { namespace draw {
    using namespace ace::draw::detail;

    namespace {
        uint8_t vertex_ao(bool side1, bool side2, bool corner) {
            if (side1 && side2) { return 3; }
            return side1 + side2 + corner;
        }

        template<typename T>
        uint8_t get_vis(T &set, glm::ivec3 pos) {
            if (!set.count(pos)) return 0;

            uint8_t vis = 0;
            if (!set.count({ pos.x - 1, pos.y, pos.z })) vis |= 1 << int(Face::LEFT);
            if (!set.count({ pos.x + 1, pos.y, pos.z })) vis |= 1 << int(Face::RIGHT);
            if (!set.count({ pos.x, pos.y - 1, pos.z })) vis |= 1 << int(Face::BACK);
            if (!set.count({ pos.x, pos.y + 1, pos.z })) vis |= 1 << int(Face::FRONT);
            if (!set.count({ pos.x, pos.y, pos.z - 1 })) vis |= 1 << int(Face::TOP);
            if (!set.count({ pos.x, pos.y, pos.z + 1 })) vis |= 1 << int(Face::BOTTOM);
            return vis;
        }

        bool is_solid_waterless(AceMap &map, const int x, const int y, const int z) {
            return (z != MAP_Z - 1) && map.is_solid(x, y, z, true);
        }

        void get_surrounding(AceMap &map, const int x, const int y, const int z, std::array<bool, 27> &ret) {
            int i = 0;
            for (int az = z + 1; az >= z - 1; az--) {
            //  for(int ay = y - 1; ay <= y + 1; ay++) {
            //      for(int ax = x - 1; ax <= ax + 1; ax++) { yes im the devil

                ret[i++] = is_solid_waterless(map, x - 1, y - 1, az);
                ret[i++] = is_solid_waterless(map, x, y - 1, az);
                ret[i++] = is_solid_waterless(map, x + 1, y - 1, az);

                ret[i++] = is_solid_waterless(map, x - 1, y, az);
                ret[i++] = is_solid_waterless(map, x, y, az);
                ret[i++] = is_solid_waterless(map, x + 1, y, az);

                ret[i++] = is_solid_waterless(map, x - 1, y + 1, az);
                ret[i++] = is_solid_waterless(map, x, y + 1, az);
                ret[i++] = is_solid_waterless(map, x + 1, y + 1, az);
            }
        }


        // TODO merge with gen_faces
        void gen_faces_with_ao(const float x, const float y, const float z, const uint8_t vis, const glm::u8vec3 &color, gl::experimental::mesh<VXLVertex> &mesh, const std::array<bool, 27> &surr) {
            const float x0 = x, x1 = x + 1.0f;
            const float y0 = -z - 1.0f, y1 = -z;
            const float z0 = y, z1 = y + 1.0f;

            uint8_t ao[4];
            if (vis & 1 << int(Face::LEFT)) {
                ao[0] = vertex_ao(surr[9], surr[21], surr[18]);
                ao[1] = vertex_ao(surr[15], surr[21], surr[24]);
                ao[2] = vertex_ao(surr[3], surr[9], surr[0]);
                ao[3] = vertex_ao(surr[3], surr[15], surr[6]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x0, y0, z0 }, color, 0, ao[2] },
                    { { x0, y0, z1 }, color, 0, ao[3] },
                    { { x0, y1, z1 }, color, 0, ao[1] },
                    { { x0, y1, z0 }, color, 0, ao[0] },
                    flipped
                );
            }
            if (vis & 1 << int(Face::RIGHT)) {
                ao[0] = vertex_ao(surr[17], surr[23], surr[26]);
                ao[1] = vertex_ao(surr[11], surr[23], surr[20]);
                ao[2] = vertex_ao(surr[5], surr[17], surr[8]);
                ao[3] = vertex_ao(surr[5], surr[11], surr[2]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x1, y0, z0 }, color, 1, ao[3] },
                    { { x1, y1, z0 }, color, 1, ao[1] },
                    { { x1, y1, z1 }, color, 1, ao[0] },
                    { { x1, y0, z1 }, color, 1, ao[2] },
                    flipped
                );
            }
            if (vis & 1 << int(Face::BACK)) {
                ao[0] = vertex_ao(surr[11], surr[19], surr[20]);
                ao[1] = vertex_ao(surr[9], surr[19], surr[18]);
                ao[2] = vertex_ao(surr[1], surr[11], surr[2]);
                ao[3] = vertex_ao(surr[1], surr[9], surr[0]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x0, y0, z0 }, color, 2, ao[3] },
                    { { x0, y1, z0 }, color, 2, ao[1] },
                    { { x1, y1, z0 }, color, 2, ao[0] },
                    { { x1, y0, z0 }, color, 2, ao[2] },
                    flipped
                );
            }
            if (vis & 1 << int(Face::FRONT)) {
                ao[0] = vertex_ao(surr[15], surr[25], surr[24]);
                ao[1] = vertex_ao(surr[17], surr[25], surr[26]);
                ao[2] = vertex_ao(surr[7], surr[15], surr[6]);
                ao[3] = vertex_ao(surr[7], surr[17], surr[8]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x0, y0, z1 }, color, 3, ao[2] },
                    { { x1, y0, z1 }, color, 3, ao[3] },
                    { { x1, y1, z1 }, color, 3, ao[1] },
                    { { x0, y1, z1 }, color, 3, ao[0] },
                    flipped
                );
            }
            if (vis & 1 << int(Face::TOP)) {
                ao[0] = vertex_ao(surr[21], surr[19], surr[18]);
                ao[1] = vertex_ao(surr[19], surr[23], surr[20]);
                ao[2] = vertex_ao(surr[21], surr[25], surr[24]);
                ao[3] = vertex_ao(surr[23], surr[25], surr[26]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x0, y1, z0 }, color, 4, ao[0] },
                    { { x0, y1, z1 }, color, 4, ao[2] },
                    { { x1, y1, z1 }, color, 4, ao[3] },
                    { { x1, y1, z0 }, color, 4, ao[1] },
                    flipped
                );
            }
            if (vis & 1 << int(Face::BOTTOM)) {
                ao[0] = vertex_ao(surr[3], surr[7], surr[6]);
                ao[1] = vertex_ao(surr[5], surr[7], surr[8]);
                ao[2] = vertex_ao(surr[1], surr[3], surr[0]);
                ao[3] = vertex_ao(surr[1], surr[5], surr[2]);
                bool flipped = ao[0] + ao[3] <= ao[1] + ao[2];

                mesh.add_quad(
                    { { x0, y0, z0 }, color, 5, ao[2] },
                    { { x1, y0, z0 }, color, 5, ao[3] },
                    { { x1, y0, z1 }, color, 5, ao[1] },
                    { { x0, y0, z1 }, color, 5, ao[0] },
                    flipped
                );
            }
        }

        void gen_faces(const float x, const float y, const float z, const uint8_t vis, const glm::u8vec3 &color, gl::experimental::mesh<VXLVertex> &mesh) {
            const float x0 = x, x1 = x + 1.0f;
            const float y0 = -z - 1.0f, y1 = -z;
            const float z0 = y, z1 = y + 1.0f;

            // vis = 0b11111111;

            if (vis & 1 << int(Face::LEFT)) {
                mesh.add_quad(
                    { { x0, y0, z0 }, color, 0 },
                    { { x0, y0, z1 }, color, 0 },
                    { { x0, y1, z1 }, color, 0 },
                    { { x0, y1, z0 }, color, 0 }
                );
            }
            if (vis & 1 << int(Face::RIGHT)) {
                mesh.add_quad(
                    { { x1, y0, z0 }, color, 1 },
                    { { x1, y1, z0 }, color, 1 },
                    { { x1, y1, z1 }, color, 1 },
                    { { x1, y0, z1 }, color, 1 }
                );
            }
            if (vis & 1 << int(Face::BACK)) {
                mesh.add_quad(
                    { { x0, y0, z0 }, color, 2 },
                    { { x0, y1, z0 }, color, 2 },
                    { { x1, y1, z0 }, color, 2 },
                    { { x1, y0, z0 }, color, 2 }
                );
            }
            if (vis & 1 << int(Face::FRONT)) {
                mesh.add_quad(
                    { { x0, y0, z1 }, color, 3 },
                    { { x1, y0, z1 }, color, 3 },
                    { { x1, y1, z1 }, color, 3 },
                    { { x0, y1, z1 }, color, 3 }
                );
            }
            if (vis & 1 << int(Face::TOP)) {
                mesh.add_quad(
                    { { x0, y1, z0 }, color, 4 },
                    { { x0, y1, z1 }, color, 4 },
                    { { x1, y1, z1 }, color, 4 },
                    { { x1, y1, z0 }, color, 4 }
                );
            }
            if (vis & 1 << int(Face::BOTTOM)) {
                mesh.add_quad(
                    { { x0, y0, z0 }, color, 5 },
                    { { x1, y0, z0 }, color, 5 },
                    { { x1, y0, z1 }, color, 5 },
                    { { x0, y0, z1 }, color, 5 }
                );
            }
        }
    }

    VXLBlocks::VXLBlocks(glm::u8vec3 color): centroid(0) {
        this->_gen_faces(glm::vec3(0), color, 0b11111111);
        this->mesh.upload();
    }

    void VXLBlocks::update(const std::vector<VXLBlock> &blocks, const glm::vec3 &center) {
        this->centroid = center;

        std::unordered_set<glm::ivec3> bmap;
        for (const VXLBlock &block : blocks) {
            bmap.emplace(block.position.x, block.position.y, block.position.z);
        }

        for (const VXLBlock &block : blocks) {
            this->_gen_faces(block.position, glm::u8vec3(unpack_argb(block.color)), get_vis(bmap, block.position));
        }
        this->mesh.upload();
    }

    void VXLBlocks::update(const std::unordered_map<glm::ivec3, uint32_t> &blocks, const glm::vec3 &center) {
        this->centroid = center;

        for (auto &i : blocks) {
            this->_gen_faces(i.first, glm::u8vec3(unpack_argb(i.second)), get_vis(blocks, i.first));
        }
        this->mesh.upload();
    }

    void VXLBlocks::update(const std::unordered_map<glm::ivec3, glm::u8vec3> &blocks, const glm::vec3 &center) {
        this->centroid = center;

        for (auto &i : blocks) {
            this->_gen_faces(i.first, i.second, get_vis(blocks, i.first));
        }
        this->mesh.upload();
    }

    void VXLBlocks::draw(gl::ShaderProgram& s) const {
        s.uniform("model", model_matrix(this->position, this->rotation, this->scale));
        this->mesh.draw();
    }

    inline void VXLBlocks::_gen_faces(const glm::vec3 &position, const glm::u8vec3 &color, uint8_t visibility) {
        glm::vec3 vert = position - this->centroid;
        gen_faces(vert.x, vert.y, vert.z, visibility, color, this->mesh);
    }

    Pillar::Pillar(AceMap &map, size_t x, size_t y) : dirty(true), map(map), x(x), y(y) {
    }

    void Pillar::update(bool use_ao) {
        std::array<bool, 27> surrounding;
        // bool use_ao = this->scene.client.config.json["graphics"].value("ambient_occlusion", true);
        for (size_t ax = this->x; ax < this->x + PILLAR_SIZE; ax++) {
            for (size_t ay = this->y; ay < this->y + PILLAR_SIZE; ay++) {
                for (size_t az = 0; az < MAP_Z; az++) {
                    uint8_t vis = this->map.get_vis(ax, ay, az, true);
                    if (az == MAP_Z - 1) vis &= 1 << int(Face::TOP);
                    if (vis == 0) continue;

                    uint8_t r, g, b, a;
                    unpack_bytes(this->map.get_color(ax, ay, az), &a, &r, &g, &b);

                    const glm::u8vec3 color(glm::vec3{ r, g, b } * (this->sunblock(ax, ay, az) / 127.f) * (a / 127.f));

                    if(use_ao) {
                        get_surrounding(this->map, ax, ay, az, surrounding);
                        gen_faces_with_ao(ax, ay, az, vis, color, this->mesh, surrounding);
                    } else {
                        gen_faces(ax, ay, az, vis, color, this->mesh);
                    }
                }
            }
        }

        this->mesh.upload();
        this->dirty = false;
    }

    void Pillar::draw(bool update_with_ao) {
        if (this->dirty) this->update(update_with_ao);

        this->mesh.draw();
    }

    int Pillar::sunblock(int x, int y, int z) const {
        int dec = 18;
        int i = 127;

        while (dec && z) {
            if (this->map.is_solid(x, --y, --z, true))
                i -= dec;
            dec -= 2;
        }
        return i;
    }
    
    MapRenderer::MapRenderer(AceMap &map, bool use_ao) : map(map), use_ao(use_ao) {
        map.add_listener(*this);
        this->gen_pillars();
    }

    void MapRenderer::draw(gl::ShaderProgram &shader, Camera &camera) {
        for (auto &p : this->pillars) {
// #ifndef NDEBUG
//             if (p.contains(draw2vox(this->scene.cam.position))) {
//                 this->scene.debug.draw_cube({ p.x + 8, -32, p.y + 8 }, { PILLAR_SIZE, 64, PILLAR_SIZE }, { 1, 0, 0 });
//             }
// #endif
            if (camera.box_in_frustum(p.x, 0, p.y, p.x + PILLAR_SIZE, -64, p.y + PILLAR_SIZE)) {
                p.draw(this->use_ao);
            }
        }
    }

    void MapRenderer::on_block_changed(int x, int y, int z, AceMap &map) {
        this->get_pillar(x - 1, y, z).dirty = true;
        this->get_pillar(x + 1, y, z).dirty = true;
        this->get_pillar(x, y - 1, z).dirty = true;
        this->get_pillar(x, y + 1, z).dirty = true;
    }

    void MapRenderer::all_changed(AceMap &map) {
        for (auto &pillar : this->pillars) {
            pillar.dirty = true;
        }
    }

    void MapRenderer::gen_pillars() {
        this->pillars.clear();
        this->pillars.reserve((MAP_X / PILLAR_SIZE) * (MAP_Y / PILLAR_SIZE));
        for (size_t x = 0; x < MAP_X / PILLAR_SIZE; x++) {
            for (size_t y = 0; y < MAP_Y / PILLAR_SIZE; y++) {
                this->pillars.emplace_back(this->map, x * PILLAR_SIZE, y * PILLAR_SIZE);
            }
        }
    }

//     bool DrawMap::build_point(const int x, const int y, const int z, glm::u8vec3 color, bool force) {
//         if (!force) {
//             if (!valid_build_pos(x, y, z)) return false;
//
//             std::vector<glm::ivec3> neighbors;
//             this->add_neighbors(neighbors, x, y, z);
//             if (neighbors.empty()) return false;
//         }
//
}}
