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

        virtual void update(double dt) { }
        virtual void draw() { }

        virtual void on_key(SDL_Scancode scancode, int modifiers, bool pressed) { }
        virtual void on_mouse_button(int button, bool pressed) { }
        virtual void on_mouse_motion(int x, int y, int dx, int dy) { }

        virtual bool on_text_typing(const std::string &text) { return true; }
        virtual void on_text_finished(bool cancelled) { }

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
        void enable(bool enable) {
            if (this->_enabled == enable) return;
            this->_enabled = enable; this->layout();
        }

        glm::vec2 position() const { return this->_pos; }
        glm::vec2 size() const { return this->_size; }
        bool enabled() const { return this->_enabled; }

        virtual void layout() {}

        // i have no idea what im doing lol
        template<typename TFunc>
        void on(const std::string &event, TFunc&& func) {
            this->handlers.emplace(event, std::forward<TFunc>(func));
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

    struct IconButton : BaseButton {
        IconButton(scene::Scene &s, glm::vec2 position, glm::vec2 size, const std::string &image = "", glm::vec2 image_scale = {}, const std::string &button = "button_square");

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
    
    struct TextButton : BaseButton {
        TextButton(scene::Scene &s, glm::vec2 position, glm::vec2 size,
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
            this->scale = this->size().y / this->bar->h();
        };

        int range{ 100 }, value{ 50 };
        float padding{ 2.f };
    private:
        draw::SpriteGroup *bar;
        float scale;
    };

    struct NavBar : InputHandler {
        NavBar(scene::Scene &scene, glm::vec2 size = {40, 40}, int font_size = 30) :
            InputHandler(scene),
            back(scene, {}, size, "BACK", "ui/common_elements/nav_bar/back_icon.png", Align::BOTTOM_LEFT, font_size),
            menu(scene, {}, size, "MENU", "ui/common_elements/nav_bar/main_menu_icon.png", Align::BOTTOM_CENTER, font_size),
            quit(scene, {}, size, "QUIT", "ui/common_elements/nav_bar/quit_icon.png", Align::BOTTOM_RIGHT, font_size) {

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

        template<typename TFunc>
        void on_back(TFunc&& func) {
            this->back.on("press_start", std::forward<TFunc>(func));
        }

        template<typename TFunc>
        void on_menu(TFunc&& func) {
            this->menu.on("press_start", std::forward<TFunc>(func));
        }

        template<typename TFunc>
        void on_quit(TFunc&& func) {
            this->quit.on("press_start", std::forward<TFunc>(func));
        }

        TextButton back, menu, quit;
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

        glm::vec2 offset() const {
            return this->_image.get_position();
        }

        glm::vec2 scale() const {
            return this->_image.scale;
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

    struct ScrollBar : GUIWidget {
        ScrollBar(scene::Scene &scene, glm::vec2 position, glm::vec2 size);

        void draw() override;
        void update(double dt) override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_mouse_motion(int x, int y, int dx, int dy) override;

        int value() const { return this->_thumb_position; }
        int range() const { return this->_thumb_range; }

        void set_value(int value) { this->_thumb_position = value; this->update_thumb(); }
        void set_range(int range) { this->_thumb_range = range; this->update_thumb(); }

        void set_thumb(int value, int range) { this->_thumb_position = value; this->_thumb_range = range; this->update_thumb(); }
    private:
        void layout() override;
        void update_thumb();

        IconButton up, down;
        draw::Sprite bottom, mid, top;
        int _thumb_position{ 0 }, _thumb_range{ 50 };
    };

    // I was considering writing a generic List widget but that's gonna be a huge pain in the ass in C++ (tuples and crap)
    // Instead I'll just make this specific for the server browser, and expand later if *really* needed.

    // what a mess of a class lol
    struct ListCtrl : GUIWidget {
        enum class SortOrder {
            NAME, PLAYERS, MAP, MODE, PING
        };

        struct Column {
            draw::Text header;
            float x_pos;

            Column(draw::Font *f, std::string header, float x_pos) : header(f, std::move(header), glm::vec3(232, 207, 78) / 255.f), x_pos(x_pos) {
            }

            void set_position(glm::vec2 pos) {
                this->header.position = pos;
                this->header.position.x += this->x_pos;
            }

            bool pressed(glm::vec2 mpos) const;
        };

        struct ServerEntry {
            std::string name; int players, max_players; std::string map; std::string mode; int ping; std::string identifier;
        };

        ListCtrl(scene::Scene &scene, glm::vec2 position, glm::vec2 size, glm::vec2 scale, ScrollBar *sb = nullptr);

        void add(ServerEntry s) { this->entries.emplace_back(std::move(s)); this->update_sb(); }
        void clear() { this->entries.clear(); this->_selected = -1; this->offset = 0; this->update_sb(); }
        size_t count() const { return this->entries.size(); }

        void draw() override;
        void layout() override;
        void on_mouse_button(int button, bool pressed) override;
        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        
        const ServerEntry *selected() const;
    private:
        void sort();
        void set_offset(int offset);
        void update_sb() const;
        // oof so ugly

        ScrollBar *sb;

        Font *column_font, *entry_font;
        glm::vec2 scale;
        float col_height;
        Column name, players, map, mode, ping;
        
        std::vector<ServerEntry> entries;

        int offset = 0, _selected = -1;
        SortOrder sort_order{ SortOrder::PLAYERS };
        bool sort_ascending{ true };
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

}}
