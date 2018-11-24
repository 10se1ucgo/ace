#pragma once
#ifdef USE_PYBIND11
#include "draw/font.h"
#include "scene/scene.h"
#include "camera.h"
#include "draw/map.h"
#include "draw/debug.h"

#include "bindings.h"
#include <pybind11/embed.h>
#undef snprintf // pyerrors.h why!!!!!!!

namespace ace { namespace scene {
    class MapEditor final : public Scene {
    public:
        MapEditor(GameClient &client, const std::string &map_name = "normandie.vxl");
        ~MapEditor();

        void start() override;
        void update(double dt) override;
        void draw() override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_button(int button, bool pressed) override;

        void on_text_finished(bool cancelled) override;

        Face hitcast(glm::ivec3 *hit) const;

        template<typename... TArgs>
        void script_hook_run(const char *hook_name, TArgs &&...args) {
            try {
                this->scripts.attr(hook_name)(std::forward<TArgs>(args)...);
            } catch (pybind11::error_already_set &e) {
                fmt::print(stderr, "Error in Python script hook!\n{}", e.what());
            }
        }

        Camera cam;

        EditableMap map;
        draw::MapRenderer map_renderer;

        draw::DebugDraw debug;
        draw::FontManager &fonts;
        gl::experimental::ubo<Uniforms3D> uniforms;

        glm::mat4 projection_2d;

        pybind11::scoped_interpreter guard;
        pybind11::object scripts;
    };
}}
#endif