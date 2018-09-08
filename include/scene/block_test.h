#pragma once
#include "scene/scene.h"
#include "camera.h"
#include "draw/map.h"

namespace ace { namespace scene {
    struct BlockTest final : Scene {
    public:
        BlockTest(GameClient &client, const std::string &map_name = "normandie.vxl");

        void start() override;
        void update(double dt) override;
        void draw() override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_button(int button, bool pressed) override;

        Camera cam;

        AceMap map;
        draw::MapRenderer map_renderer;

        gl::experimental::ubo<Uniforms3D> uniforms;

        glm::mat4 projection_2d;
    };
}}
