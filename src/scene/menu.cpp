#include "scene/menu.h"

#include <functional>

#include "scene/game.h"
#include <SDL_image.h>
#include "draw/sprite.h"

namespace ace { namespace scene {
    inline float wave(float x, float a, float b) {
        return (sin(x) * (a - b) + (a + b)) / 2.f;
    }
    

    struct MainMenu : Menu {
        explicit MainMenu(scene::Scene &scene) : Menu(scene),
            background(scene.client.sprites.get("main.png")), splash(scene.client.sprites.get("splash.png")),
            button1(this->add<draw::ImageTextButton>("weebs", glm::vec2{ 400, 400 }, glm::vec2{ 256, 128 }, "stencil.ttf", 55)),
            button2(this->add<draw::ImageTextButton>("test2", glm::vec2{ 100, 400 }, glm::vec2{ 300, 55 })) {

            background->order = -1;

            glm::vec2 size = this->scene.client.size();
            splash.position = size / 2.f;
            splash.alignment = draw::Align::BOTTOM_CENTER;
            splash.scale *= 0.5f;

            button1->on("press_end", []() { fmt::print("You pressed the button!\n"); });
        }

        void update(double dt) override {
            Menu::update(dt);
        
            splash.scale = glm::vec2(wave(this->scene.time / 0.45f, 0.49f, 0.52f));
            button2->set_size({ wave(this->scene.time, 200, 300),  wave(this->scene.time * 2, 55, 75) });
        }

        void draw() override {
            Menu::draw();

            background->draw({ 1, 1, 1, 1 }, { 0, 0 }, 0, this->scene.client.size() / glm::vec2(background->w, background->h));
            splash.draw();
        }

        draw::SpriteGroup *background;
        draw::Sprite splash;
        draw::ImageTextButton *button1, *button2;
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
        this->client.fonts.draw(projection, client.shaders->text);
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
