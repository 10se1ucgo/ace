#include "scene/loading.h"
#include <functional>

namespace ace { namespace scene {

    LoadingScene::LoadingScene(GameClient &client, const char *host, int port): Scene(client) {
        client.net->connect(host, port);
    }

    LoadingScene::LoadingScene(GameClient& client, const std::string& address): Scene(client) {
        client.net->connect(address);
    }

    LoadingScene::~LoadingScene() {
    }

    void LoadingScene::draw() {
    }

    void LoadingScene::update(double dt) {
        if (!client.net->connected) return;

        int max = client.net->map_writer.vec.capacity();
        int cur = client.net->map_writer.vec.size();
        fmt::print("{} | {} out of {}\n", cur / float(max) * 100, cur, max);
    }

    void LoadingScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
    }

    void LoadingScene::on_mouse_motion(int x, int y, int dx, int dy) {
    }

    void LoadingScene::on_mouse_button(int button, bool pressed) {
    }
}}
