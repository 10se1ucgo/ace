#include "draw/map.h"

#include "scene/game.h"

namespace ace { namespace draw {
    namespace {
        struct Vertex {
            glm::vec3 vertex;
            glm::vec3 color;
            GLubyte face;
        };
        

        void gen_faces(const float x, const float y, const float z, const uint8_t vis, const glm::vec3 color, std::vector<Vertex> &v) {
            const float x0 = x, x1 = x + 1.0f;
            const float y0 = -z - 1.0f, y1 = -z;
            const float z0 = y, z1 = y + 1.0f;

            // vis = 0b11111111;

            if (vis & 1 << int(Face::LEFT)) {
                v.push_back({ { x0, y0, z0 }, color, 0 });
                v.push_back({ { x0, y1, z0 }, color, 0 });
                v.push_back({ { x0, y0, z1 }, color, 0 });
                v.push_back({ { x0, y0, z1 }, color, 0 });
                v.push_back({ { x0, y1, z0 }, color, 0 });
                v.push_back({ { x0, y1, z1 }, color, 0 });
            }
            if (vis & 1 << int(Face::RIGHT)) {
                v.push_back({ { x1, y0, z0 }, color, 1 });
                v.push_back({ { x1, y0, z1 }, color, 1 });
                v.push_back({ { x1, y1, z0 }, color, 1 });
                v.push_back({ { x1, y1, z0 }, color, 1 });
                v.push_back({ { x1, y0, z1 }, color, 1 });
                v.push_back({ { x1, y1, z1 }, color, 1 });
            }
            if (vis & 1 << int(Face::BACK)) {
                v.push_back({ { x0, y0, z0 }, color, 2 });
                v.push_back({ { x1, y0, z0 }, color, 2 });
                v.push_back({ { x0, y1, z0 }, color, 2 });
                v.push_back({ { x0, y1, z0 }, color, 2 });
                v.push_back({ { x1, y0, z0 }, color, 2 });
                v.push_back({ { x1, y1, z0 }, color, 2 });
            }
            if (vis & 1 << int(Face::FRONT)) {
                v.push_back({ { x0, y0, z1 }, color, 3 });
                v.push_back({ { x0, y1, z1 }, color, 3 });
                v.push_back({ { x1, y0, z1 }, color, 3 });
                v.push_back({ { x1, y0, z1 }, color, 3 });
                v.push_back({ { x0, y1, z1 }, color, 3 });
                v.push_back({ { x1, y1, z1 }, color, 3 });
            }
            if (vis & 1 << int(Face::TOP)) {
                v.push_back({ { x0, y1, z0 }, color, 4 });
                v.push_back({ { x1, y1, z0 }, color, 4 });
                v.push_back({ { x0, y1, z1 }, color, 4 });
                v.push_back({ { x0, y1, z1 }, color, 4 });
                v.push_back({ { x1, y1, z0 }, color, 4 });
                v.push_back({ { x1, y1, z1 }, color, 4 });
            }
            if (vis & 1 << int(Face::BOTTOM)) {
                v.push_back({ { x0, y0, z0 }, color, 5 });
                v.push_back({ { x0, y0, z1 }, color, 5 });
                v.push_back({ { x1, y0, z0 }, color, 5 });
                v.push_back({ { x1, y0, z0 }, color, 5 });
                v.push_back({ { x0, y0, z1 }, color, 5 });
                v.push_back({ { x1, y0, z1 }, color, 5 });
            }
        }
    }

    VXLBlocks::VXLBlocks(const std::vector<VXLBlock> &blocks, const glm::vec3 &center) : scale(1), rotation(0), position(0) {
        this->centroid += center;

        std::vector<Vertex> v;
        v.reserve(blocks.size() * sizeof(Vertex));
        for (const VXLBlock &block : blocks) {
            uint8_t r, g, b, a;
            unpack_color(block.color, &a, &r, &g, &b);

            gen_faces(
                block.position.x - this->centroid.x,
                block.position.y - this->centroid.y,
                block.position.z - this->centroid.z,
                block.vis, glm::vec3{ r, g, b } / 255.f, v
            );
        }
        this->vertices = v.size();

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), v.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, vertex)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        // glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, face)));
        glEnableVertexAttribArray(2);
    }

    void VXLBlocks::draw(const glm::mat4& pv, ShaderProgram& s) const {
        s.uniform("mvp", pv * model_matrix(this->position, this->rotation, this->scale));
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices);
    }

    Pillar::Pillar(AceMap &map, size_t x, size_t y) : dirty(true), map(map), x(x), y(y), vbo_size(0), vertices(0) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, vertex)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, face)));
        glEnableVertexAttribArray(2);
    }

    void Pillar::update() {
        std::vector<Vertex> v;
        v.reserve(4096 * 24);

        for (size_t ax = this->x; ax < this->x + PILLAR_SIZE; ax++) {
            for (size_t ay = this->y; ay < this->y + PILLAR_SIZE; ay++) {
                for (size_t az = 0; az < MAP_Z; az++) {
                    uint8_t vis = map.get_vis(ax, ay, az, true);
                    if (az == MAP_Z - 1) vis = vis & (1 << int(Face::TOP));
                    if (vis == 0) continue;

                    const uint32_t col = map.get_color(ax, ay, az);
                    uint8_t r, g, b, a;
                    unpack_color(col, &a, &r, &g, &b);

                    gen_faces(ax, ay, az, vis, (glm::vec3{ r, g, b } * (map.sunblock(ax, ay, az) / 127.f) * (a / 127.f)) / 255.f, v);
                }
            }
        }
        this->vertices = v.size();

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (true /* this->vertices * sizeof(Vertex) > vbo_size */) {
            glInvalidateBufferData(GL_ARRAY_BUFFER);
            glBufferData(GL_ARRAY_BUFFER, this->vertices * sizeof(Vertex), v.data(), GL_DYNAMIC_DRAW);
            vbo_size = this->vertices * sizeof(Vertex);
        } else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertices * sizeof(Vertex), v.data());
        }

        this->dirty = false;
    }

    void Pillar::draw() {
        if (dirty) this->update();

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices);
    }

    std::unique_ptr<uint8_t[]> read_file(const std::string &file_path) {
        FILE *f = fopen(file_path.c_str(), "rb");
        if (!f) THROW_ERROR("COULD NOT READ MAP FILE {}\n", file_path);

        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        rewind(f);
        auto buf = std::make_unique<uint8_t[]>(len);
        fread(buf.get(), len, 1, f);
        fclose(f);
        return buf;
    }

    DrawMap::DrawMap(scene::GameScene &s, const std::string &file_path) : DrawMap(s, read_file(file_path).get()) {
    }

    DrawMap::DrawMap(scene::GameScene &s, uint8_t *buf) : AceMap(buf), scene(s), overview(this->get_overview()) {
        this->gen_pillars();
    }

    void DrawMap::update(double dt) {
        for (auto i = damage_queue.begin(); i != damage_queue.end();) {
            if (scene.time >= i->first) {
                int x = i->second.x, y = i->second.y, z = i->second.z;
                if(this->get_solid(x, y, z)) {
                    this->set_point(x, y, z, true, (0x7F << 24) | this->colors[get_pos(x, y, z)]);
                }
                i = damage_queue.erase(i);
            } else {
                ++i;
            }
        }
    }

    void DrawMap::draw(const glm::vec3 &position, ShaderProgram &shader) {
        for (auto &p : this->pillars) {
            if (this->scene.cam.box_in_frustum(p.x, 0, p.y, p.x + PILLAR_SIZE, -64, p.y + PILLAR_SIZE)) {
                p.draw();
            }
        }
    }

    Pillar &DrawMap::get_pillar(const int x, const int y, const int z) {
        int xp = (x & MAP_X - 1) / PILLAR_SIZE;
        int yp = (y & MAP_Y - 1) / PILLAR_SIZE;
        return this->pillars[xp * (MAP_Y / PILLAR_SIZE) + yp];
    }

    void DrawMap::gen_pillars() {
        pillars.clear();
        pillars.reserve((MAP_X / PILLAR_SIZE) * (MAP_Y / PILLAR_SIZE));
        for (size_t x = 0; x < MAP_X / PILLAR_SIZE; x++) {
            for (size_t y = 0; y < MAP_Y / PILLAR_SIZE; y++) {
                pillars.emplace_back(*this, x * PILLAR_SIZE, y * PILLAR_SIZE);
            }
        }
    }


    bool DrawMap::set_point(const int x, const int y, const int z, const bool solid, const uint32_t color) {
        bool ok = AceMap::set_point(x, y, z, solid, color);

        this->get_pillar(x - 1, y, z).dirty |= ok;
        this->get_pillar(x + 1, y, z).dirty |= ok;
        this->get_pillar(x, y - 1, z).dirty |= ok;
        this->get_pillar(x, y + 1, z).dirty |= ok;
        // pillars dont render edges even across pillar boundries
        // so you have to update adjacent pillars if the destroyed block shares a face with another pillar.
        // TODO: shadows dont update across chunks. the origin block can be ~18 blocks a way, so maybe update all chunks within
        // 18 blocks? or only in the direction of shadows (increasing y)

        if (x == 0 || y == 0 || x == MAP_X - 1 || y == MAP_Y - 1 || !(x & 63) || !(y & 63)) {
            return ok;
        }

        // todo this sucks massive donkey balls why am i updating the texture 1 by 1 lol
        // i should batch updates and update by region the next frame, not per block
        uint8_t argb[4];
        unpack_color(this->get_color(x, y, this->get_z(x, y)), argb + 0, argb + 1, argb + 2, argb + 3);
        glBindTexture(GL_TEXTURE_2D, this->overview.tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, argb + 1);

        return ok;
    }

    bool DrawMap::build_point(const int x, const int y, const int z, const glm::ivec3& color, bool force) {
        if (!force) {
            if (!valid_build_pos(x, y, z)) return false;

            std::vector<glm::ivec3> neighbors;
            this->add_neighbors(neighbors, x, y, z);
            if (neighbors.empty()) return false;
        }

        return this->set_point(x, y, z, true, pack_color(0x7F, color.r, color.g, color.b));
    }

    bool DrawMap::destroy_point(const int x, const int y, const int z, std::vector<VXLBlock> &destroyed) {
        if (!valid_build_pos(x, y, z)) return false;

        bool ok = this->set_point(x, y, z, false, 0);

        std::vector<glm::ivec3> neighbors;
        this->add_neighbors(neighbors, x, y, z);
        for (const auto &node : neighbors) {
            if (valid_build_pos(node.x, node.y, node.z)) {
                this->check_node(node.x, node.y, node.z, true, destroyed);
            }
        }

        return ok;
    }

    bool DrawMap::damage_point(int x, int y, int z, uint8_t damage) {
        if (!valid_build_pos(x, y, z) || !this->get_solid(x, y, z)) return false;

        uint32_t color = this->get_color(x, y, z);
        int health = color >> 24;
        health -= damage;
        if(health <= 0) {
            return true;
        }
        color = ((health & 0xFF) << 24) | (color & 0x00FFFFFF);
        this->set_point(x, y, z, true, color);
        damage_queue.push_back({ scene.time + 10, {x, y, z} });
        return false;
    }
    
    draw::SpriteGroup DrawMap::get_overview() {
        auto pixels(std::make_unique<uint8_t[]>(MAP_X * MAP_Y * 3));
        int p = 0;
        for(int y = 0; y < MAP_Y; y++) {
            for(int x = 0; x < MAP_X; x++) {
                uint8_t r, g, b;
                if(x == 0 || y == 0 || x == MAP_X - 1 || y == MAP_Y - 1) {
                    r = g = b = 0;
                } else if((x & 63) == 0 || (y & 63) == 0) {
                    r = g = b = 255;
                } else {
                    uint8_t a;
                    unpack_color(this->get_color(x, y, this->get_z(x, y)), &a, &r, &g, &b);
                }
                
                pixels[p++] = r; pixels[p++] = g; pixels[p++] = b;
            }
        }
        return draw::SpriteGroup(SDL_CreateRGBSurfaceFrom(pixels.get(), MAP_X, MAP_Y, 24, 3 * MAP_X, 0xFF, 0xFF << 8, 0xFF << 16, 0));
    }
}}
