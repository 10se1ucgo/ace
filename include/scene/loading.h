#pragma once
#include "scene/scene.h"
#include "net/net.h"
#include "draw/gui.h"
#include "draw/font.h"


namespace ace { namespace scene {
    struct LoadingFrame : draw::GUIPanel {
        explicit LoadingFrame(scene::Scene &scene);

        void draw() override;

        draw::BigFrame frame;
        draw::Sprite content;
        draw::ProgressBar *progress_bar;
        draw::Button *start_button;
        draw::Text status_text;
        draw::NavBar *nav_bar;
    };

    class LoadingScene final : public Scene {
    public:
//        using Scene::Scene;
//        LoadingScene(GameClient &client, const char *host, int port);
        LoadingScene(GameClient &client, const std::string &address);
        ~LoadingScene();

        void draw() override;
        void update(double dt) override;

        bool on_text_typing(const std::string &text) override;
        void on_text_finished() override;

        void on_window_resize(int ow, int oh) override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;

        void on_net_event(net::NetState event) override;
        void on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) override;
        
        void start_game();

        glm::mat4 projection;
        draw::Font *font, *aldo;

        net::Server server;
        std::vector<std::pair<net::PACKET, std::unique_ptr<net::Loader>>> saved_loaders;
        std::unique_ptr<GameScene> game_scene;
        
        draw::SpriteGroup *background;
        float background_alpha{1.0};
        LoadingFrame frame;
    };
}}
