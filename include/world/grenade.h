#pragma once
#include "glm/glm.hpp"

#include "world/world.h"
#include "kv6.h"

namespace ace { namespace world {
    struct Grenade : WorldObject {
        Grenade(scene::GameScene &scene, glm::vec3 position, glm::vec3 velocity, float fuse);

        bool update(double dt) override;
        void draw() override;

        KV6 mesh;
        glm::vec3 p, v;
        float fuse;
    private:
        bool move(double dt);
    };
}}
