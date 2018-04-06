#include "scene/loading.h"

#include <functional>

#include "scene/game.h"

namespace ace { namespace scene {
    LoadingScene::LoadingScene(GameClient& client, const std::string &address):
        Scene(client),
        font(client.fonts.get("fixedsys.ttf", 48, false)),
        aldo(client.fonts.get("AldotheApache.ttf", 48)),
        server(address),
        background(client.sprites.get("main.png")),
        big_frame(client.sprites.get("ui/common_elements/frames/ui_frame_large.png")),
        loading_frame(client.sprites.get("ui/game_loading/game_loading_content_frames.png")),
        progress_bar(*this, {}, {}),
        frame_text(this->aldo, "LOADING...", glm::vec3(1), glm::vec2(1), draw::Align::CENTER) {


        this->on_window_resize(0, 0);

        if (this->client.net.state == net::NetState::DISCONNECTED || this->client.net.state == net::NetState::UNCONNECTED)
            this->client.toggle_text_input();

        this->big_frame.alignment = this->loading_frame.alignment = draw::Align::CENTER;
        this->big_frame.position = loading_frame.position = this->client.size() / 2.f;
        this->big_frame.scale = this->loading_frame.scale = (this->client.size() / big_frame.size()) * 0.9f;

        this->background->order = -100;
        this->big_frame.group->order = -11;
        this->loading_frame.group->order = -10;

        this->progress_bar.set_position(loading_frame.get_position(draw::Align::TOP_LEFT) + glm::vec2(70, 700) * loading_frame.scale);
        this->progress_bar.set_size(glm::vec2{ 650, 70 } * loading_frame.scale);
        this->frame_text.position = big_frame.get_position(draw::Align::TOP_LEFT) + glm::vec2(580, 60) * big_frame.scale;

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        
    }

    LoadingScene::~LoadingScene() {
    }

    void LoadingScene::draw() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        this->background->draw({ 1, 1, 1, 1 }, { 0, 0 }, 0, this->client.size() / glm::vec2(this->background->w, this->background->h));
        this->big_frame.draw();
        this->frame_text.draw_shadowed();
        this->loading_frame.draw();
        this->progress_bar.draw();

        
        if(this->client.net.state == net::NetState::UNCONNECTED || this->client.net.state == net::NetState::DISCONNECTED) {
            std::string str;
            if (this->client.text_input_active())
                str = fmt::format("ENTER NAME: {}_", this->client.input_buffer);
            else
                str = "DISCONNECTED";
            font->draw(str, { client.width() / 2.f, client.height() / 2.f }, { 1, 1, 1 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        }

        this->client.shaders->sprite.bind();
        this->client.shaders->sprite.uniform("projection", this->projection);
        this->client.sprites.draw(this->client.shaders->sprite);

        this->client.shaders->text.bind();
        this->client.fonts.draw(this->projection, this->client.shaders->text);
    }

    void LoadingScene::update(double dt) {
        Scene::update(dt);
        progress_bar.value = wave(this->time, 0, 101);
        this->progress_bar.value = client.net.map_writer.vec.size();
        this->progress_bar.range = std::max(1u, client.net.map_writer.vec.capacity());
    }

    bool LoadingScene::on_text_typing(const std::string &text) {
        return client.input_buffer.length() < 15;
    }

    void LoadingScene::on_text_finished() {
        if (client.net.state != net::NetState::DISCONNECTED && client.net.state != net::NetState::UNCONNECTED) return;

        client.net.connect(server);
        client.net.ply_name = this->client.input_buffer;
    }

    void LoadingScene::on_window_resize(int ow, int oh) {
        projection = glm::ortho(0.f, float(client.width()), float(client.height()), 0.0f);
    }

    void LoadingScene::on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) {
        if(type == net::PACKET::StateData) {
            auto &client = this->client;
            auto buf(net::inflate(client.net.map_writer.vec.data(), client.net.map_writer.vec.size()));
            auto saved_loaders(std::move(this->saved_loaders));

            // hey so im pretty sure calling client.set_scene invalidates this object (this->scene.reset() destroys this)
            // so im gonna quickly copy/move all of the important stuff out of the class before we destroy it
            // is this bad design? absolutely. i think.
            client.set_scene<GameScene>(*reinterpret_cast<net::StateData *>(packet.get()), client.net.ply_name, buf.data());
            for (auto &pkt : saved_loaders) {
                client.scene->on_packet(pkt.first, std::move(pkt.second));
            }
        } else {
            saved_loaders.emplace_back(type, std::move(packet));
        }
    }

    void LoadingScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        if (scancode == SDL_SCANCODE_ESCAPE && pressed) this->client.quit = true;
    }
//
//    void LoadingScene::on_mouse_motion(int x, int y, int dx, int dy) {
//    }
//
//    void LoadingScene::on_mouse_button(int button, bool pressed) {
//    }
}}
