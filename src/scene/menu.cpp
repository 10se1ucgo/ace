#include "scene/menu.h"

#include <functional>

#include "game_client.h"
#include "scene/game.h"
#include "scene/loading.h"
#include <SDL_image.h>
#include "draw/sprite.h"

namespace ace { namespace scene {
    struct MainMenu : draw::GUIPanel {
        draw::SpriteGroup *background;
        draw::Sprite splash, menu_frame;
        draw::Button *button1, *button2;
        draw::BitmapButton *button3;
        draw::ProgressBar *pb;
        draw::IconButton *nav_quit;

        explicit MainMenu(scene::Scene &scene) : GUIPanel(scene),
            background(scene.client.sprites.get("main.png")), splash(scene.client.sprites.get("splash.png")),
            menu_frame(scene.client.sprites.get("ui/main_menu/frame_main_menu.png")),
            button1(this->add<draw::Button>("Play Online", glm::vec2{}, glm::vec2{}, 36)),
            button2(this->add<draw::Button>("Settings", glm::vec2{}, glm::vec2{}, 36)),
            button3(this->add<draw::BitmapButton>(glm::vec2{0, 0}, glm::vec2(0))),
            pb(this->add<draw::ProgressBar>(glm::vec2{0, 0}, glm::vec2{0, 0})),
            nav_quit(this->add<draw::IconButton>(glm::vec2{}, glm::vec2{40}, "QUIT", "ui/common_elements/nav_bar/quit_icon.png")) {

            this->background->order = draw::Layer::BACKGROUND;

            this->splash.position = this->scene.client.size() / 2.f;
            this->splash.alignment = draw::Align::BOTTOM_CENTER;
            this->splash.scale *= 0.5f;

            this->menu_frame.group->order = draw::Layer::FRAME;
            this->menu_frame.scale = glm::vec2(0.4f * (float(this->scene.client.height()) / this->menu_frame.group->h));
            this->menu_frame.position = { this->scene.client.width() / 2.f, this->scene.client.height() - 225 * this->menu_frame.scale.y };
            this->menu_frame.alignment = draw::Align::CENTER;
            
            auto p = this->menu_frame.get_position(draw::Align::TOP_LEFT);
            this->button1->set_position(p + glm::vec2(60) * this->menu_frame.scale);
            this->button1->set_size(glm::vec2{ 445, 100 } * this->menu_frame.scale);

            this->button2->set_position(this->button1->position() + glm::vec2(0, this->button1->size().y + 5));
            this->button2->set_size(this->button1->size());

            this->button1->on("press_end", [this]() {
                //this->scene.client.tasks.call_later(0.1, [this]() {
                    std::string ip("aos://1252610391:32850:0.75");
                    this->scene.client.set_scene<ace::scene::LoadingScene>(ip);
                //});
            });

            this->nav_quit->set_position(p + glm::vec2(280, 340) * this->menu_frame.scale);
            this->nav_quit->set_size(glm::vec2{ 40, 40 } * this->menu_frame.scale);
            this->nav_quit->on("press_start", [this]() { this->scene.client.quit = true; });
        }

        void update(double dt) override {
            GUIPanel::update(dt);
        
            this->splash.scale = glm::vec2(wave(this->scene.time, 0.49f, 0.52f));
//            this->button2->set_size({ wave(this->scene.time, 200, 300), wave(this->scene.time * 2, 55, 75) });
            this->pb->value = int(wave(this->scene.time, 0.f, 100.f));
        }

        void draw() override {
            GUIPanel::draw();

            this->background->draw({ 1, 1, 1, 1 }, { 0, 0 }, 0, this->scene.client.size() / glm::vec2(background->w, background->h));
            this->splash.draw();
            this->menu_frame.draw();
        }
    };

    MainMenuScene::MainMenuScene(GameClient &client) : Scene(client),
        projection(glm::ortho(0.f, float(this->client.width()), float(this->client.height()), 0.0f)) {

        SDL_Surface *cursor(draw::load_image("png/cursor.png").first);
        SDL_SetCursor(SDL_CreateColorCursor(cursor, 0, 0));

        this->set_menu<MainMenu>();
        this->client.sound.play_music("test.ogg");
    }

    MainMenuScene::~MainMenuScene() {
//        this->client.sound.stop_music();
    }

    void MainMenuScene::draw() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->menu->draw();

        this->client.shaders->sprite.bind();
        this->client.shaders->sprite.uniform("projection", this->projection);
        this->client.sprites.draw(this->client.shaders->sprite);

        this->client.shaders->text.bind();
        this->client.fonts.draw(this->projection, this->client.shaders->text);
    }

    void MainMenuScene::update(double dt) {
        Scene::update(dt);
        this->menu->update(dt);
    }

    void MainMenuScene::on_mouse_motion(int x, int y, int dx, int dy) {
        this->menu->on_mouse_motion(x, y, dx, dy);
    }

    void MainMenuScene::on_mouse_button(int button, bool pressed) {
        this->menu->on_mouse_button(button, pressed);
    }

    void MainMenuScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        this->menu->on_key(scancode, modifiers, pressed);
    }
}}
