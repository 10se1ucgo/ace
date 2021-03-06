#pragma once
#include <string>

#include "SDL.h"
#include "nlohmann/json.hpp"

#include "sound/sound.h"
#include "draw/font.h"
#include "util/event.h"
#include "net/net.h"
#include "net/url.h"
#include "draw/sprite.h"
#include "scene/scene.h"
#include "draw/kv6.h"

#ifndef USE_PHYSICS_SUBSTEP
# define USE_PHYSICS_SUBSTEP 0
#endif

namespace ace {
    namespace scene {
        struct Scene;
    }

    enum class WINDOW_STYLE {
        WINDOWED,
        FULLSCREEN,
        FULLSCREEN_BORDERLESS
    };

    struct GameConfig {
        GameConfig();
        GameConfig(const std::string &file_name);
        void read(const std::string &file_name);

        SDL_Scancode get_key(const std::string &key);
        SDL_Scancode get_key(const std::string &key, SDL_Scancode default_value) {
            try {
                return this->get_key(key);
            } catch (nlohmann::json::out_of_range &) {
                return default_value;
            }
        }

        nlohmann::json json;
    private:
        // SDL_GetScancodeFromName is quite slow (strcmp over every key name)
        std::unordered_map<std::string, SDL_Scancode> keybind_to_scancode;
    };

    class GameClient {
    public:
        GameClient(std::string caption /*, int w = 800, int h = 600, WINDOW_STYLE style = WINDOW_STYLE::WINDOWED */);
        ~GameClient();
        ACE_NO_COPY_MOVE(GameClient)

        void run();
        void quit() { this->_quit = true; }

        glm::vec2 size() const { return { w, h }; }
        int width() const { return w; }
        int height() const { return h; }

        void set_exclusive_mouse(bool exclusive) {
            SDL_SetRelativeMouseMode(exclusive ? SDL_TRUE : SDL_FALSE);
        }

        bool exclusive_mouse() const {
            return SDL_GetRelativeMouseMode();
        }

        void toggle_text_input() const {
            this->set_text_input(!this->text_input_active());
        }

        void set_text_input(bool enable) const {
            if (enable)
                SDL_StartTextInput();
            else
                SDL_StopTextInput();
        }

        bool text_input_active() const {
            return SDL_IsTextInputActive() == SDL_TRUE;
        }

        template<typename TScene, typename... TArgs, typename = std::enable_if_t<std::is_base_of<scene::Scene, TScene>::value>>
        void set_scene(TArgs&&... args) {
            this->new_scene.reset();
            this->new_scene = std::make_unique<TScene>(*this, std::forward<TArgs>(args)...);
        }

        void set_scene(std::unique_ptr<scene::Scene> scene) {
            this->new_scene.reset();
            this->new_scene = std::move(scene);
        }
        
        net::NetworkClient net;
        net::URLClient url;
        std::unique_ptr<gl::ShaderManager> shaders; 
        // unique_ptr because GL context needs to be created before the shaders can be compiled and loaded.
        draw::SpriteManager sprites;
        sound::SoundManager sound;
        util::TaskScheduler tasks;
        draw::FontManager fonts;
        draw::KV6Manager models;
        GameConfig config;

        // Input state
        struct {
            int numkeys = 0;
            const Uint8 *keys = nullptr;
            SDL_Keymod mods = KMOD_NONE;
        } keyboard;
        struct {
            int x = 0, y = 0, dx = 0, dy = 0;
            Uint32 state = 0, dstate = 0;

            bool left() const { return state & SDL_BUTTON_LMASK; }
            bool right() const { return state & SDL_BUTTON_RMASK; }
            bool middle() const { return state & SDL_BUTTON_MMASK; }
        } mouse;

        std::string input_buffer;
        size_t input_cursor = 0;

        
        double time = 0.0;

        std::unique_ptr<scene::Scene> scene{ nullptr }; // very bad idea??
    private:
        void draw() const;
        void update(double dt);
        void update_fps();

        void poll_events();
        void handle_key_press(const SDL_Event &event);
        void handle_window_event(const SDL_Event &event);
        void handle_text_input(const SDL_Event &event);

        int w, h;
        struct {
            double last_update = 0.0;
            int frames = 0;
        } fps_counter;

        double fixed_update_accumulator = 0.0;

        SDL_Window *window;
        SDL_GLContext context;
        std::string window_title;

        std::unique_ptr<scene::Scene> new_scene{ nullptr };

        bool _quit{ false };
    };
}
