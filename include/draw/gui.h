#pragma once
#include "draw/sprite.h"
#include "scene/scene.h"

namespace ace { namespace draw {
    // im considering putting this in its own namespace (ace::gui) but we'll see

    struct InputHandler {
        InputHandler(scene::Scene &scene) : scene(scene) { };

        virtual ~InputHandler() = default;

        virtual void update(double dt) {};
        virtual void draw() = 0;
        virtual void on_key(SDL_Scancode scancode, int modifiers, bool pressed) { };
        virtual void on_mouse_button(int button, bool pressed) { };
        virtual void on_mouse_motion(int x, int y, int dx, int dy) { };

        scene::Scene &scene;
    };

    struct GUIWidget : InputHandler {
        explicit GUIWidget(scene::Scene &scene) : InputHandler(scene) {
        }

        virtual void set_position(glm::vec2 position) = 0;
        virtual void set_size(glm::vec2 size) = 0;

        virtual glm::vec2 position() = 0;
        virtual glm::vec2 size() = 0;

        // i have no idea what im doing lol

        template<typename TFunc, typename... TArgs>
        void on(const std::string &event, TFunc&& func, TArgs&&... args) {
            this->handlers.emplace(event, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
        }
    protected:
        virtual void fire(const std::string &event) {
            const auto handler(this->handlers.find(event));
            if(handler != this->handlers.end()) {
                handler->second();
            }
        }

        std::unordered_map<std::string, std::function<void()>> handlers;
    };

    struct Button : GUIWidget {
        Button(scene::Scene &scene, glm::vec2 position, glm::vec2 size);

        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;

        void set_position(glm::vec2 position) override { this->_pos = position; }
        void set_size(glm::vec2 size) override { this->_size = size; }

        glm::vec2 position() override { return this->_pos; }
        glm::vec2 size() override { return this->_size; }
    protected:
        glm::vec2 _pos, _size;
        bool hovering{ false }, pressed{ false };
    };
    
    struct ButtonImages {
        ButtonImages(scene::Scene &s, const std::string &button_name);

        draw::SpriteGroup *left, *mid, *right;
        draw::SpriteGroup *left_hover, *mid_hover, *right_hover;
        draw::SpriteGroup *left_press, *mid_press, *right_press;
    };

    struct ImageTextButton : Button {
        ImageTextButton(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, const std::string &font = "stencil.ttf", int font_size = 48, std::string button_name = "button_large");

        void draw() override;

        void set_position(glm::vec2 position) override;
        void set_size(glm::vec2 size) override;
    protected:
        draw::Sprite left, mid, right;
        draw::Font *font;
        draw::Text label;
        ButtonImages images;

        void fire(const std::string &event) override {
            Button::fire(event);
            this->update_images();
        }

        void update_images();
        void update_position();
    };
}}
