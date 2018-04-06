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
        explicit GUIWidget(scene::Scene &scene, glm::vec2 position, glm::vec2 size) :
            InputHandler(scene),
            _pos(position),
            _size(size) {
        }

        void set_position(glm::vec2 position) { this->_pos = position; this->layout(); }
        void set_size(glm::vec2 size) { this->_size = size; this->layout(); }

        glm::vec2 position() { return this->_pos; }
        glm::vec2 size() { return this->_size; }

        virtual void layout() {}

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

        glm::vec2 _pos, _size;
        std::unordered_map<std::string, std::function<void()>> handlers;
    };

    struct GUIPanel : draw::InputHandler {
        GUIPanel(scene::Scene &scene) : InputHandler(scene) {
        }

        void update(double dt) override { for (auto &w : this->widgets) w->update(dt); }
        void draw() override { for (auto &w : this->widgets) w->draw(); }
        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override { for (auto &w : this->widgets) w->on_key(scancode, modifiers, pressed); }
        void on_mouse_button(int button, bool pressed) override { for (auto &w : this->widgets) w->on_mouse_button(button, pressed); }
        void on_mouse_motion(int x, int y, int dx, int dy) override { for (auto &w : this->widgets) w->on_mouse_motion(x, y, dx, dy); }

        template<typename TWidget, typename... TArgs, typename = std::enable_if_t<std::is_base_of<draw::GUIWidget, TWidget>::value>>
        TWidget *add(TArgs&&... args) {
            std::unique_ptr<TWidget> obj = std::make_unique<TWidget>(this->scene, std::forward<TArgs>(args)...);
            TWidget *ptr = obj.get(); // moving the unique_ptr shouldnt have an effect on the actual pointer
            widgets.emplace_back(std::move(obj));
            return ptr;
        }

        std::vector<std::unique_ptr<GUIWidget>> widgets;
    };

    struct BaseButton : GUIWidget {
        BaseButton(scene::Scene &scene, glm::vec2 position, glm::vec2 size) : GUIWidget(scene, position, size) { }

        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
    protected:
        bool hovering{ false }, pressed{ false };
    };

    struct BitmapButton : BaseButton {
        BitmapButton(scene::Scene &s, glm::vec2 position, glm::vec2 size, const std::string &image = "", const std::string &button = "button_square");

        void draw() override;

        void layout() override {
            BaseButton::layout();
            this->update_position();
        }
    protected:
        draw::SpriteGroup *normal, *hover, *press;
        draw::Sprite button, image;

        void fire(const std::string &event) override {
            this->update_images();
            BaseButton::fire(event);
        }

        void update_images();
        void update_position();
    };
    

    struct Button : BaseButton {
        Button(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, const std::string &font = "stencil.ttf", int font_size = 48, const std::string &button_name = "button_large");

        void draw() override;

        void layout() override {
            BaseButton::layout();
            this->update_position();
        }
    protected:
        draw::Sprite left, mid, right;
        draw::Text label;

        struct ButtonImages {
            ButtonImages(scene::Scene &s, const std::string &button_name);

            draw::SpriteGroup *left, *mid, *right;
            draw::SpriteGroup *left_hover, *mid_hover, *right_hover;
            draw::SpriteGroup *left_press, *mid_press, *right_press;
        } images;

        void fire(const std::string &event) override {
            BaseButton::fire(event);
            this->update_images();
        }

        void update_images();
        void update_position();
    };

    struct ProgressBar : GUIWidget {
        ProgressBar(scene::Scene &scene, glm::vec2 position, glm::vec2 size, const std::string &image = "ui/game_loading/loading_bar_bullet.png");

        void draw() override;

        void layout() override {
            GUIWidget::layout();
            this->scale = this->size().y / this->bar->h;
        };

        int range{ 100 }, value{ 50 };
        float padding{ 2.f };
    private:
        draw::SpriteGroup *bar;
        float scale;
    };
}}
