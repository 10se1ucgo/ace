#pragma once
#include "scene/scene.h"
#include "net.h"
#include "draw/gui.h"


namespace ace { namespace scene {
    struct Menu : draw::InputHandler {
        Menu(scene::Scene &scene): InputHandler(scene) {
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

        std::vector<std::unique_ptr<draw::GUIWidget>> widgets;
    };

    class MainMenuScene final : public Scene {
    public:
        MainMenuScene(GameClient &client);
        ~MainMenuScene();

        void draw() override;
        void update(double dt) override;
       
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;

        template<typename TMenu, typename... TArgs, typename = std::enable_if_t<std::is_base_of<Menu, TMenu>::value>>
        void set_menu(TArgs&&... args) {
            this->menu.reset();
            this->menu = std::make_unique<TMenu>(*this, std::forward<TArgs>(args)...);
        }

        glm::mat4 projection;
    private:
        std::unique_ptr<Menu> menu;
    };
}}
