#pragma once
#include "SDL.h"
#include "scene.h"
#include "net.h"


namespace ace { namespace scene {
    class LoadingScene : public Scene {
    public:
        using Scene::Scene;
        LoadingScene(GameClient &client, const char *host, int port);
        LoadingScene(GameClient &client, const std::string &address);
        ~LoadingScene();

        void draw() override;
        void update(double dt) override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
    };
}}
