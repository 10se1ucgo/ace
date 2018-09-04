#include "world/world.h"

namespace ace { namespace world {

    World::World(scene::GameScene &scene) : map_renderer(scene), scene(scene) {
    }

    void World::update(double dt) {
    }

    void World::draw() {
    }

    bool World::build_block(int x, int y, int z, glm::u8vec3 color, bool force) {
    }

    bool World::destroy_block(int x, int y, int z, net::ACTION type) {
    }

    bool World::destroy_block(int x, int y, int z, std::vector<VXLBlock> &destroyed) {
        // destroy the node's path!
        // if (destroy) {
        //     // need to iter twice to get proper visflags.
        //     for (const auto &pos : marked) {
        //         if (this->get_solid(pos.x, pos.y, pos.z)) {
        //             destroyed.push_back({ pos, this->get_color(pos.x, pos.y, pos.z), this->get_vis(pos.x, pos.y, pos.z) });
        //         }
        //     }
        //
        //     for (const auto &pos : marked) {
        //         this->set_point(pos.x, pos.y, pos.z, false, 0);
        //     }
        // }
    }

    void World::check_floating(int x, int y, int z, std::vector<VXLBlock> &floating) {
        this->marked.clear();
        this->nodes.clear();
        this->nodes.emplace_back(x, y, z);

        while (!nodes.empty()) {
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
    }

    bool World::damage_block(int x, int y, int z, int damage, bool allow_destroy) {
    }
}}