#include "world/world.h"

namespace ace { namespace world {

    World::World(scene::GameScene &scene) : scene(scene), map_renderer(scene) {
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
    }

    void World::check_node(int x, int y, int z, std::vector<VXLBlock> &destroyed) {
        marked.clear();
        nodes.clear();
        nodes.emplace_back(x, y, z);

        while (!nodes.empty()) {
            const glm::ivec3 &node = nodes.back();
            x = node.x; y = node.y; z = node.z;
            nodes.pop_back();
            if (z >= MAP_Z - 2) {
                return;
            }

            // already visited?
            const auto ret = marked.emplace(x, y, z);
            if (ret.second) {
                this->add_neighbors(nodes, x, y, z);
            }
        }

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

    bool World::damage_block(int x, int y, int z, int damage, bool allow_destroy) {
    }
}}