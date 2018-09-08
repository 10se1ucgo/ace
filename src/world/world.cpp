#include "world/world.h"
#include "world/falling_blocks.h"
#include "scene/game.h"

namespace ace { namespace world {

    World::World(scene::GameScene &scene, uint8_t *buf) : map(buf), map_renderer(map), scene(scene) {
    }

    void World::update(double dt) {
        for (auto i = this->damaged_blocks.begin(); i != this->damaged_blocks.end();) {
            if (scene.time >= i->first) {
                int x = i->second.x, y = i->second.y, z = i->second.z;
                this->map.set_color(x, y, z, (0x7F << 24) | (this->map.get_color(x, y, z) & 0x00FFFFFF));
                i = this->damaged_blocks.erase(i);
            } else {
                ++i;
            }
        }
    }

    void World::draw() {
        this->map_renderer.draw(this->scene.shaders.map, this->scene.cam);
    }

    bool World::build_block(int x, int y, int z, glm::u8vec3 color, bool force) {
        bool ok = this->map.set_block(x, y, z, true, pack_bytes(0x7F, color.r, color.g, color.b));
        return ok;
    }

    bool World::destroy_block(int x, int y, int z, net::ACTION type) {
        std::vector<VXLBlock> v;
        bool ok = this->destroy_block(x, y, z, v);
        switch (type) {
        case net::ACTION::SPADE:
            ok |= this->destroy_block(x, y, z + 1, v);
            ok |= this->destroy_block(x, y, z - 1, v);
            break;
        case net::ACTION::GRENADE:
            for (int i = x - 1; i < x + 2; i++) {
                for (int j = y - 1; j < y + 2; j++) {
                    for (int k = z - 1; k < z + 2; k++) {
                        ok |= this->destroy_block(i, j, k, v);
                    }
                }
            }
            break;
        default:
            break;
        }
        if (!v.empty()) {
            this->scene.create_object<world::FallingBlocks>(v);
        }
        return ok;
    }

    bool World::damage_block(int x, int y, int z, int damage, bool allow_destroy) {
        uint32_t color;
        if (!draw::valid_build_pos(x, y, z) || !this->map.get_block(x, y, z, &color)) return false;

        int health = color >> 24;
        health -= damage;
        if (health <= 0) {
            if(allow_destroy) this->destroy_block(x, y, z, net::ACTION::DESTROY);
            return true;
        }

        color = ((std::max(health, 0) & 0xFF) << 24) | (color & 0x00FFFFFF);
        this->map.set_color(x, y, z, color);
        this->damaged_blocks.push_back({ this->scene.time + 10, { x, y, z } });
        return false;
    }

    bool World::destroy_block(int x, int y, int z, std::vector<VXLBlock> &destroyed) {
        if (!draw::valid_build_pos(x, y, z)) return false;

        bool ok = this->map.set_solid(x, y, z, false);

        std::vector<glm::ivec3> neighbors;
        this->add_neighboring_nodes(neighbors, x, y, z);
        for (const auto &node : neighbors) {
            if (draw::valid_build_pos(node.x, node.y, node.z)) {
                this->check_floating(node.x, node.y, node.z, destroyed);
            }
        }

        return ok;
    }

    void World::check_floating(int x, int y, int z, std::vector<VXLBlock> &floating, bool destroy) {
        this->marked.clear();
        this->nodes.clear();
        this->nodes.emplace_back(x, y, z);

        while (!this->nodes.empty()) {
            glm::ivec3 node = this->nodes.back();
            this->nodes.pop_back();
            if (node.z >= MAP_Z - 2) {
                return;
            }

            // already visited?
            const auto ret = this->marked.emplace(node);
            if (ret.second) {
                this->add_neighboring_nodes(this->nodes, node.x, node.y, node.z);
            }
        }

        for (const auto pos : marked) {
            uint32_t color;
            if (this->map.get_block(pos.x, pos.y, pos.z, &color)) {
                floating.push_back({ pos, color });
                this->map.set_solid(pos.x, pos.y, pos.z, !destroy);
            }
        }
    }

    bool World::clipworld(int x, int y, int z) const {
        if (x < 0 || x >= MAP_X || y < 0 || y >= MAP_Y)
            return false;

        if (z == MAP_Z - 1)
            z--;
        else if (z >= MAP_Z)
            return true;
        else if (z < 0)
            return false;

        return this->map.is_solid(x, y, z);
    }

    bool World::clipbox(int x, int y, int z) const {
        if (x < 0 || x >= MAP_X || y < 0 || y >= MAP_Y)
            return true;

        if (z == MAP_Z - 1)
            z--;
        else if (z >= MAP_Z)
            return true;
        else if (z < 0)
            return false;

        return this->map.is_solid(x, y, z);
    }
}}
