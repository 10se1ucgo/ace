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
        World(scene::GameScene &scene);

        void update(double dt);
        void draw();

        bool build_block(int x, int y, int z, glm::u8vec3 color, bool force = false);

        bool destroy_block(int x, int y, int z, net::ACTION type);
        
        // Damage a block, subtracting `damage` from its health. If health <= 0 after, optionally destroy. -> success
        bool damage_block(int x, int y, int z, int damage, bool allow_destroy);

    private:
        // Destroy block AND check for floating structures, adding any removed floating blocks to the `destroyed` vector. -> success
        bool destroy_block(int x, int y, int z, std::vector<VXLBlock> &destroyed);

        void check_node(int x, int y, int z, std::vector<VXLBlock> &destroyed);

        AceMap map;
        draw::MapRenderer map_renderer;

        // Floating block detection
        std::vector<glm::ivec3> nodes;
        std::unordered_set<glm::ivec3> marked;

        scene::GameScene &scene;
    };
}}