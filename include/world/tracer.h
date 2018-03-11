#pragma once
#include "glm/glm.hpp"

#include "world/world.h"
#include "draw/map.h"
#include "kv6.h"

namespace ace { namespace world {
    struct Tracer : WorldObject {
        Tracer(scene::GameScene &scene, const std::string &mesh, glm::vec3 position, glm::vec3 orientation);

        bool update(double dt) override;
        void draw() override;

        KV6 mesh;
        glm::vec3 position, orientation;

        constexpr static float SPEED = 256.f, LIFESPAN = 0.5f;
        float time_alive = 0.0f;
    };
}}
