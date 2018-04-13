#pragma once
#include <functional>

#include "draw/sprite.h"
#include "scene/scene.h"
#include "draw/font.h"


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
        void enable(bool enable) { this->_enabled = enable; this->layout(); }

        glm::vec2 position() const { return this->_pos; }
        glm::vec2 size() const { return this->_size; }
        bool enabled() const { return this->_enabled; }

        virtual void layout() {}

        // i have no idea what im doing lol

        template<typename TFunc, typename... TArgs>
        void on(const std::string &event, TFunc&& func, TArgs&&... args) {
            this->handlers.emplace(event, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
        }

    protected:
        virtual void fire(const std::string &event);

        glm::vec2 _pos, _size;
        bool _enabled{true};
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

        template<typename TWidget, typename... TArgs, typename = std::enable_if_t<std::is_base_of<draw::InputHandler, TWidget>::value>>
        TWidget *add(TArgs&&... args) {
            std::unique_ptr<TWidget> obj = std::make_unique<TWidget>(this->scene, std::forward<TArgs>(args)...);
            TWidget *ptr = obj.get(); // moving the unique_ptr shouldnt have an effect on the actual pointer
            widgets.emplace_back(std::move(obj));
            return ptr;
        }

        std::vector<std::unique_ptr<InputHandler>> widgets;
    };

    struct BaseButton : GUIWidget {
        BaseButton(scene::Scene &scene, glm::vec2 position, glm::vec2 size) : GUIWidget(scene, position, size) { }

        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
    protected:
        bool hovering{ false }, pressed{ false };

        virtual bool hit_test(glm::vec2 mouse_position);
    };

    struct Button : BaseButton {
        Button(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, int font_size = 48, const std::string &font = "stencil.ttf", const std::string &button_name = "button_large");

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
            this->update_images();
            BaseButton::fire(event);
        }

        void update_images();
        void update_position();
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
    
    struct IconButton : BaseButton {
        IconButton(scene::Scene &s, glm::vec2 position, glm::vec2 size,
                   std::string text = "", const std::string &icon = "", draw::Align alignment = Align::BOTTOM_CENTER,
                   int font_size = 30, const std::string &font = "AldotheApache.ttf");

        void draw() override;
        void layout() override;
    protected:
        glm::vec2 hitbox_pos, hitbox_size;
        draw::Align alignment;

        void fire(const std::string &event) override {
            this->layout();
            BaseButton::fire(event);
        }

        bool hit_test(glm::vec2 mouse_position) override;

        draw::Sprite icon;
        draw::Text text;
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

    struct NavBar : InputHandler {
        NavBar(scene::Scene &scene, glm::vec2 size = {}) :
            InputHandler(scene),
            back(scene, {}, size, "BACK", "ui/common_elements/nav_bar/back_icon.png", Align::BOTTOM_LEFT),
            menu(scene, {}, size, "MENU", "ui/common_elements/nav_bar/main_menu_icon.png", Align::BOTTOM_CENTER),
            quit(scene, {}, size, "QUIT", "ui/common_elements/nav_bar/quit_icon.png", Align::BOTTOM_RIGHT) {

            // glm::vec2{}, glm::vec2{40}, "QUIT", "ui/common_elements/nav_bar/quit_icon.png"
        }

        void draw() override {
            this->back.draw(); this->menu.draw(); this->quit.draw();
        }

        void update(double dt) override {
            this->back.update(dt); this->menu.update(dt); this->quit.update(dt);
        }

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override {
            this->back.on_key(scancode, modifiers, pressed); this->menu.on_key(scancode, modifiers, pressed); this->quit.on_key(scancode, modifiers, pressed);
        }

        void on_mouse_button(int button, bool pressed) override {
            this->back.on_mouse_button(button, pressed); this->menu.on_mouse_button(button, pressed); this->quit.on_mouse_button(button, pressed);
        }

        void on_mouse_motion(int x, int y, int dx, int dy) override {
            this->back.on_mouse_motion(x, y, dx, dy); this->menu.on_mouse_motion(x, y, dx, dy); this->quit.on_mouse_motion(x, y, dx, dy);
        }

        void set_positions(glm::vec2 left, glm::vec2 middle, glm::vec2 right) {
            this->back.set_position(left); this->menu.set_position(middle); this->quit.set_position(right);
        }

        void set_scales(glm::vec2 scale) {
            this->back.set_size(scale); this->menu.set_size(scale); this->quit.set_size(scale);
        }

        IconButton back, menu, quit;
    };

    struct Frame {
        Frame(scene::Scene &scene, const std::string &image, std::string title, glm::vec2 position,
              glm::vec2 title_offset, float size);

        void draw() {
            this->_image.draw();
            this->_title.draw_shadowed();
        }

        void set_title(const std::string &title) {
            this->_title.set_str(title);
        }

        const draw::Sprite &image() const {
            return this->_image;
        }

        const draw::Text &text() const {
            return this->_title;
        }

        void position_navbar(NavBar &nb, glm::vec2 scale) const {
            nb.set_positions(this->left, this->middle, this->right);
            nb.set_scales(scale * this->_image.scale);
        }
    protected:
        void set_navbar_positions(glm::vec2 left, glm::vec2 middle, glm::vec2 right) {
            this->left = left; this->middle = middle; this->right = right;
        }
        glm::vec2 left, middle, right;
        draw::Sprite _image;
        draw::Text _title;
    };


    struct BigFrame : Frame {
        BigFrame(scene::Scene &scene, std::string title, glm::vec2 position, float size) :
            Frame(scene, "ui/common_elements/frames/ui_frame_large.png", std::move(title), position, { 580, 60 }, size) {

            auto tl = this->_image.get_position(draw::Align::TOP_LEFT);
            auto &s = this->_image.scale;
            this->set_navbar_positions(tl + glm::vec2(50, 860) * s, tl + glm::vec2(580, 860) * s, tl + glm::vec2(1100, 860) * s);
        }
    };

    struct MediumFrame : Frame {
        MediumFrame(scene::Scene &scene, std::string title, glm::vec2 position, float size) :
            Frame(scene, "ui/common_elements/frames/ui_frame_medium.png", std::move(title), position, { 570, 60 }, size) {

            auto tl = this->_image.get_position(draw::Align::TOP_LEFT);
            auto &s = this->_image.scale;
            this->set_navbar_positions(tl + glm::vec2(50, 715) * s, tl + glm::vec2(580, 715) * s, tl + glm::vec2(1100, 715) * s);
        }
    };

    struct SmallFrame : Frame {
        SmallFrame(scene::Scene &scene, std::string title, glm::vec2 position, float size) :
            Frame(scene, "ui/common_elements/frames/ui_frame_small.png", std::move(title), position, { 430, 60 }, size) {
        }
    };
//
//    using s = Frame;
}}
