#pragma once
#include "scene/scene.h"
#include "net.h"
#include "draw/gui.h"


namespace ace { namespace scene {
    class MainMenuScene final : public Scene {
    public:
        MainMenuScene(GameClient &client);
        ~MainMenuScene();

        void draw() override;
        void update(double dt) override;
       
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;

        template<typename TMenu, typename... TArgs, typename = std::enable_if_t<std::is_base_of<draw::GUIPanel, TMenu>::value>>
        void set_menu(TArgs&&... args) {
            this->menu.reset();
            this->menu = std::make_unique<TMenu>(*this, std::forward<TArgs>(args)...);
        }

        glm::mat4 projection;
    private:
        std::unique_ptr<draw::GUIPanel> menu;
    };
}}
