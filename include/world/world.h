#pragma once

#include "scene/scene.h"
//#include "scene/game.h"

namespace ace { namespace world {
    struct WorldObject {
        WorldObject(scene::GameScene &scene) : scene(scene) {}
        virtual ~WorldObject() = default;

        virtual void draw() = 0;
        virtual bool update(double dt) = 0;

        scene::GameScene &scene;
    };
}}