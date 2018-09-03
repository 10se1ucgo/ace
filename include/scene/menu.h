#pragma once
#include "scene/scene.h"
#include "net/net.h"
#include "draw/gui.h"


namespace ace { namespace scene {
    class MainMenuScene;

    struct Menu : draw::GUIPanel {
        Menu(scene::MainMenuScene &scene);
        virtual void start() = 0;

        scene::MainMenuScene &scene;
    };

    struct MainMenu : Menu {
        explicit MainMenu(scene::MainMenuScene &scene);
        void start() override;
        void update(double dt) override;
        void draw() override;

    private:
        draw::SpriteGroup *background;
        draw::Sprite splash, menu_frame;
        draw::Button *play_button, *settings_button;
        draw::IconButton *button3;
        draw::ProgressBar *pb;
        draw::TextButton *nav_quit;
    };

    class MainMenuScene final : public Scene {
    public:
        MainMenuScene(GameClient &client);
        // ~MainMenuScene();

        void start() override;
        void draw() override;
        void update(double dt) override;
       
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;

        template<typename TMenu, typename... TArgs, typename = std::enable_if_t<std::is_base_of<draw::GUIPanel, TMenu>::value>>
        void set_menu(TArgs&&... args) {
            this->new_menu.reset();
            this->new_menu = std::make_unique<TMenu>(*this, std::forward<TArgs>(args)...);
        }

        glm::mat4 projection;
    private:
        std::unique_ptr<Menu> new_menu, menu;
    };
}}
