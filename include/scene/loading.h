#pragma once
#include "scene/scene.h"
#include "net.h"


namespace ace { namespace scene {
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

//        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
//        void on_mouse_motion(int x, int y, int dx, int dy) override;
//        void on_mouse_button(int button, bool pressed) override;

        void on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) override;
        

        glm::mat4 projection;
        draw::Font *font;

        net::Server server;
        std::vector<std::pair<net::PACKET, std::unique_ptr<net::Loader>>> saved_loaders;
    };
}}
