#include "scene/loading.h"
#include <functional>

namespace ace { namespace scene {
//    LoadingScene::LoadingScene(GameClient &client, const char *host, int port):
//        Scene(client),
//        server(std::string(host), port),
//        font(client.fonts.get("fixedsys.ttf", 48, false)) {
//    }

    LoadingScene::LoadingScene(GameClient& client, const std::string &address):
        Scene(client),
        font(client.fonts.get("fixedsys.ttf", 48, false)),
        server(address) {

        this->on_window_resize(0, 0);

        this->client.toggle_text_input();
    }

    LoadingScene::~LoadingScene() {
    }

    void LoadingScene::draw() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        std::string str;
        if(client.net->connected) {
            int max = client.net->map_writer.vec.capacity();
            int cur = client.net->map_writer.vec.size();
            str = fmt::format("{} | {} out of {}", cur / float(max) * 100, cur, max);
        } else {
            str = fmt::format("ENTER NAME: {}_", this->client.input_buffer);
        }
        font->draw(str, { client.width() / 2.f, client.height() / 2.f }, { 1, 0, 0 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
    
        client.shaders->text.bind();
        client.fonts.draw(projection, client.shaders->text);
    }

    void LoadingScene::update(double dt) {
    }

    bool LoadingScene::on_text_typing(const std::string &text) {
        return client.input_buffer.length() < 15;
    }

    void LoadingScene::on_text_finished() {
        client.net->connect(server);
        client.net->ply_name = this->client.input_buffer;
    }

    void LoadingScene::on_window_resize(int ow, int oh) {
        projection = glm::ortho(0.f, float(client.width()), float(client.height()), 0.0f);
    }

    //
//    void LoadingScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
//    }
//
//    void LoadingScene::on_mouse_motion(int x, int y, int dx, int dy) {
//    }
//
//    void LoadingScene::on_mouse_button(int button, bool pressed) {
//    }
}}
