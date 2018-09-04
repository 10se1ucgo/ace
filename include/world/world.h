#pragma once

#include "scene/scene.h"
#include "draw/map.h"

//#include "scene/game.h"

namespace ace { namespace world {
    struct WorldObject {
        WorldObject(scene::GameScene &scene) : scene(scene) {}
        virtual ~WorldObject() = default;

        virtual void draw() = 0;
        virtual bool update(double dt) = 0;

        scene::GameScene &scene;
    };

    struct World {
        World(scene::GameScene &scene, uint8_t *buf);

        void update(double dt);
        void draw();

        bool build_block(int x, int y, int z, glm::u8vec3 color, bool force = false);

        bool destroy_block(int x, int y, int z, net::ACTION type);
        
        // Damage a block, subtracting `damage` from its health. If health <= 0 after, optionally destroy. -> success
        bool damage_block(int x, int y, int z, int damage, bool allow_destroy);

        bool clipworld(int x, int y, int z) const;
        bool clipworld(float x, float y, float z) const {
            return this->clipworld(int(std::floor(x)), int(std::floor(y)), int(std::floor(z)));
        }

        bool clipbox(int x, int y, int z) const;
        bool clipbox(float x, float y, float z) const {
            return this->clipbox(int(std::floor(x)), int(std::floor(y)), int(std::floor(z)));
        }

        Face hitscan(const glm::dvec3 &p, const glm::dvec3 &d, glm::ivec3 *h) const {
            return this->map.hitscan(p, d, h);
        }
    private:
        // Destroy block AND check for floating structures, adding any removed floating blocks to the `destroyed` vector. -> success
        bool destroy_block(int x, int y, int z, std::vector<VXLBlock> &destroyed);

        void add_node(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
            if (this->map.is_solid(x, y, z))
                v.emplace_back(x, y, z);
        }

        void add_neighboring_nodes(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
            this->add_node(v, x, y, z - 1);
            this->add_node(v, x, y - 1, z);
            this->add_node(v, x, y + 1, z);
            this->add_node(v, x - 1, y, z);
            this->add_node(v, x + 1, y, z);
            this->add_node(v, x, y, z + 1);
        }

        void check_floating(int x, int y, int z, std::vector<VXLBlock> &floating, bool destroy = true);

        // Floating block detection
        std::vector<glm::ivec3> nodes;
        std::unordered_set<glm::ivec3> marked;

        AceMap map;
        draw::MapRenderer map_renderer;

        scene::GameScene &scene;
    };
}}