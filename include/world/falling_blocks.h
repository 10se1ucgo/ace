#pragma once
#include "glm/glm.hpp"

#include "world/world.h"
#include "draw/map.h"

namespace ace { namespace world {
    struct FallingBlocks : WorldObject {

        FallingBlocks(scene::GameScene& scene, const std::vector<VXLBlock>& blocks);

        bool update(double dt) override;
        void draw() override;

        draw::VXLBlocks mesh;
        glm::vec3 position, velocity, direction;

        float MAX_LIFE = 5.0f;
        float BOUNCE_DECAY = 2.0f;
        float time_alive = 0.0f, alpha = 1.0f;
    };
}}
