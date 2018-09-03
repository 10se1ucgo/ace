#include "scene/loading.h"

#include <functional>

#include "scene/game.h"
#include "game_client.h"
#include "scene/menu.h"

namespace ace { namespace scene {
    LoadingFrame::LoadingFrame(scene::Scene &scene) : GUIPanel(scene),
        frame(scene, "LOADING...", scene.client.size() / 2.f, scene.client.height() * 0.9),
        content(scene.client.sprites.get("ui/game_loading/game_loading_content_frames.png")),
        progress_bar(this->add<draw::ProgressBar>(glm::vec2(0), glm::vec2(0))),
        start_button(this->add<draw::Button>("START", glm::vec2(0), glm::vec2(0), 40)),
        status_text(scene.client.fonts.get("nevis.ttf", 16), "", glm::vec3(1), glm::vec2(1), draw::Align::CENTER),
        nav_bar(this->add<draw::NavBar>()) {

        this->content.alignment = draw::Align::CENTER;
        this->content.position = this->frame.image().position;
        this->content.scale = this->frame.image().scale;

        this->content.group->order = draw::Layer::FRAME_CONTENT;

        this->progress_bar->set_position(this->content.get_position(draw::Align::TOP_LEFT) + glm::vec2(70, 700) * this->content.scale);
        this->progress_bar->set_size(glm::vec2{ 650, 70 } * this->content.scale);
                          
        this->start_button->set_position(this->content.get_position(draw::Align::TOP_LEFT) + glm::vec2(730, 685) * this->content.scale);
        this->start_button->set_size(glm::vec2{ 350, 100 } * this->content.scale);


        this->status_text.position = this->content.get_position(draw::Align::TOP_LEFT) + glm::vec2{ 825, 550 } * this->content.scale;

        this->frame.position_navbar(*this->nav_bar, glm::vec2{40});


    }

    void LoadingFrame::draw() {
        this->frame.draw();
        this->content.draw();
        this->status_text.draw();
        GUIPanel::draw();
    }

    LoadingScene::LoadingScene(GameClient& client, const std::string &address):
        Scene(client),
        font(client.fonts.get("fixedsys.ttf", 48, false)),
        aldo(client.fonts.get("AldotheApache.ttf", 48)),
        server(address),
        background(client.sprites.get("main.png")),
        frame(*this) {

        this->background->order = draw::Layer::BACKGROUND;

        this->on_window_resize(0, 0);
    }

    void LoadingScene::start() {
        if (this->client.net.state == net::NetState::DISCONNECTED || this->client.net.state == net::NetState::UNCONNECTED) {
           this->client.net.connect(this->server);
        }

        this->frame.nav_bar->on_quit(&GameClient::quit, &this->client);
        this->frame.nav_bar->on_menu([&client = this->client]() {
            client.net.disconnect();
        });
        this->frame.nav_bar->on_back([&client = this->client]() {
            client.net.disconnect();
            auto scene = std::make_unique<MainMenuScene>(client);
            // scene->set_menu<>()
            client.set_scene(std::move(scene));
        });

        this->frame.start_button->enable(false);
        this->frame.start_button->on("press_end", &LoadingScene::start_game, this);
    }

    void LoadingScene::draw() {
        if (this->game_scene) {
            this->game_scene->draw();
        } else {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        }

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->background->draw({ 1, 1, 1, this->background_alpha }, { 0, 0 }, 0,
                               this->client.size() / glm::vec2(this->background->w(), this->background->h()));
        
        this->frame.draw();

        this->client.shaders->sprite.bind();
        this->client.shaders->sprite.uniform("projection", this->projection);
        this->client.sprites.flush(this->client.shaders->sprite);

        this->client.shaders->text.bind();
        this->client.fonts.draw(this->projection, this->client.shaders->text);
    }

    void LoadingScene::update(double dt) {
        Scene::update(dt);
        if(this->game_scene) {
            this->background_alpha = std::max(0.0, this->background_alpha - dt * 0.5);
            this->game_scene->update(dt);
        }

        this->frame.progress_bar->value = client.net.map_writer.vec.size();
        this->frame.progress_bar->range = std::max(size_t(1), client.net.map_writer.vec.capacity());
        if(this->game_scene) {
            this->frame.progress_bar->value = this->frame.progress_bar->range;
        }
        this->frame.update(dt);
        
    }

    // bool LoadingScene::on_text_typing(const std::string &text) {
    //     return client.input_buffer.length() < 15;
    // }

    void LoadingScene::on_window_resize(int ow, int oh) {
        projection = glm::ortho(0.f, float(client.width()), float(client.height()), 0.0f);
    }

    void LoadingScene::on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) {
        if(type == net::PACKET::StateData) {
            auto buf(net::inflate(client.net.map_writer.vec.data(), client.net.map_writer.vec.size()));
            this->game_scene = std::make_unique<GameScene>(this->client, *reinterpret_cast<net::StateData *>(packet.get()), this->client.config.json.value("name", "Deuce").substr(0, 15), buf.data());
            this->frame.start_button->enable(true);
            this->frame.frame.set_title("READY!");
            this->frame.status_text.set_str("Ready.");
            this->client.sound.stop_music();
        } else {
            this->saved_loaders.emplace_back(type, std::move(packet));
        }
    }

    void LoadingScene::start_game() {
        if (this->game_scene == nullptr) return;

        auto *scene = this->game_scene.get();
        this->client.set_scene(std::move(this->game_scene));

        for (auto &pkt : this->saved_loaders) {
            scene->on_packet(pkt.first, std::move(pkt.second));
        }
    }

    void LoadingScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        if (scancode == SDL_SCANCODE_ESCAPE && pressed) this->client.net.disconnect();
    }
//
    void LoadingScene::on_mouse_motion(int x, int y, int dx, int dy) {
        this->frame.on_mouse_motion(x, y, dx, dy);
    }

    void LoadingScene::on_mouse_button(int button, bool pressed) {
        this->frame.on_mouse_button(button, pressed);
    }

    void LoadingScene::on_net_event(net::NetState event) {
        switch(event) {
        case net::NetState::UNCONNECTED:
        case net::NetState::DISCONNECTED:
            this->frame.status_text.set_str(fmt::format("Disconnected: {}", net::get_disconnect_reason(this->client.net.disconnect_reason)));
            break;
        case net::NetState::CONNECTING:
            this->frame.status_text.set_str("Connecting to server...");
            break;
        case net::NetState::CONNECTED:
            this->frame.status_text.set_str("Connected, waiting for map transfer...");
            break;
        case net::NetState::MAP_TRANSFER:
            this->frame.status_text.set_str("Receiving map....");
            break;
        default:
            break;
        }
    }
}}
