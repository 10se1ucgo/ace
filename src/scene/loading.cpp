#include "scene/loading.h"

#include <functional>

#include "scene/game.h"

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

        if (client.net->state == net::NetState::DISCONNECTED || client.net->state == net::NetState::UNCONNECTED)
            this->client.toggle_text_input();

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    LoadingScene::~LoadingScene() {
    }

    void LoadingScene::draw() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        std::string str;
        switch(client.net->state) {
            case net::NetState::UNCONNECTED:
            case net::NetState::DISCONNECTED:
                str = fmt::format("ENTER NAME: {}_", this->client.input_buffer);
                break;
            case net::NetState::CONNECTING:
            case net::NetState::CONNECTED: 
                str = "CONNECTING...";
                break;
            case net::NetState::MAP_TRANSFER: {
                int max = client.net->map_writer.vec.capacity();
                int cur = client.net->map_writer.vec.size();
                str = fmt::format("{}% | {} out of {}", int(cur / float(max) * 100), cur, max);
            } break;
            default: 
                break;
        }
        font->draw(str, { client.width() / 2.f, client.height() / 2.f }, { 1, 1, 1 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
    
        client.shaders->text.bind();
        client.fonts.draw(projection, client.shaders->text);
    }

    void LoadingScene::update(double dt) {
        Scene::update(dt);
    }

    bool LoadingScene::on_text_typing(const std::string &text) {
        return client.input_buffer.length() < 15;
    }

    void LoadingScene::on_text_finished() {
        if (client.net->state != net::NetState::DISCONNECTED && client.net->state != net::NetState::UNCONNECTED) return;

        client.net->connect(server);
        client.net->ply_name = this->client.input_buffer;
    }

    void LoadingScene::on_window_resize(int ow, int oh) {
        projection = glm::ortho(0.f, float(client.width()), float(client.height()), 0.0f);
    }

    void LoadingScene::on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) {
        if(type == net::PACKET::StateData) {
            auto &client = this->client;
            auto buf(net::inflate(client.net->map_writer.vec.data(), client.net->map_writer.vec.size()));
            auto saved_loaders(std::move(this->saved_loaders));

            // hey so im pretty sure calling client.set_scene invalidates this object (this->scene.reset() destroys this)
            // so im gonna quickly copy/move all of the important stuff out of the class before we destroy it
            // is this bad design? absolutely. i think.
            client.set_scene<GameScene>(*reinterpret_cast<net::StateData *>(packet.get()), client.net->ply_name, buf.get());
            for (auto &pkt : saved_loaders) {
                client.scene->on_packet(pkt.first, std::move(pkt.second));
            }
        } else {
            saved_loaders.emplace_back(type, std::move(packet));
        }
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
