#pragma once
#include "glm/glm.hpp"

#include "draw/map.h"
#include "world/world.h"


namespace ace { namespace world {
    struct Debris {
        Debris(glm::vec3 position, glm::vec3 velocity, glm::vec3 color);

        bool update(double dt, World &world);

        glm::vec3 p, v, color;
        float life;

        constexpr static float MAX_LIFE = 2.5f;
    };

    struct DebrisGroup : WorldObject {
        DebrisGroup(scene::GameScene &scene);
        void add(glm::vec3 position, glm::u8vec3 color, float velocity_mod, int number);

        bool update(double dt) override;
        void draw() override;

        std::vector<Debris> debris;
    };
}}
