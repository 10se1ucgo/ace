#include "scene/menu.h"

#include <functional>

#include "scene/game.h"
#include <SDL_image.h>
#include "draw/sprite.h"

namespace ace { namespace scene {

    

    struct MainMenu : draw::GUIPanel {
        draw::SpriteGroup *background;
        draw::Sprite splash;
        draw::Button *weeb_button, *button2;
        draw::BitmapButton *button3;
        draw::ProgressBar *pb;

        explicit MainMenu(scene::Scene &scene) : GUIPanel(scene),
            background(scene.client.sprites.get("main.png")), splash(scene.client.sprites.get("splash.png")),
            weeb_button(this->add<draw::Button>("weebs", glm::vec2{ 400, 400 }, glm::vec2{ 256, 128 }, "stencil.ttf", 55)),
            button2(this->add<draw::Button>("test2", glm::vec2{ 100, 400 }, glm::vec2{ 300, 55 })),
            button3(this->add<draw::BitmapButton>(glm::vec2{300, 300}, glm::vec2(128), "ui/common_elements/scroll_bar/scroll_bar_arrow_right.png")),
            pb(this->add<draw::ProgressBar>(glm::vec2{200, 200}, glm::vec2{414, 43})) {

            this->background->order = -100;

            this->splash.position = this->scene.client.size() / 2.f;
            this->splash.alignment = draw::Align::BOTTOM_CENTER;
            this->splash.scale *= 0.5f;

            this->weeb_button->on("press_end", []() { fmt::print("You pressed a button!\n"); });
        }

        void update(double dt) override {
            GUIPanel::update(dt);
        
            splash.scale = glm::vec2(wave(this->scene.time / 0.45f, 0.49f, 0.52f));
            button2->set_size({ wave(this->scene.time, 200, 300),  wave(this->scene.time * 2, 55, 75) });
            pb->value = int(wave(this->scene.time, 0.f, 100.f));
        }

        void draw() override {
            GUIPanel::draw();

            background->draw({ 1, 1, 1, 1 }, { 0, 0 }, 0, this->scene.client.size() / glm::vec2(background->w, background->h));
            splash.draw();
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
        this->client.sound.stop_music();
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
