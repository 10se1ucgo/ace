#pragma once
#include <string>

#include "SDL.h"

#include "sound/sound.h"
#include "draw/font.h"
#include "util/event.h"
#include "net.h"
#include "draw/sprite.h"
#include "scene/scene.h"

namespace net {
    struct NetworkClient;
}

namespace ace {
    namespace scene {
        struct Scene;
    }

    enum class WINDOW_STYLE {
        WINDOWED,
        FULLSCREEN,
        FULLSCREEN_BORDERLESS
    };

    class GameClient {
    public:
        GameClient(std::string caption, int w = 800, int h = 600, WINDOW_STYLE style = WINDOW_STYLE::WINDOWED);
        ~GameClient();
        ACE_NO_COPY_MOVE(GameClient)

        void run();

        glm::vec2 size() const { return { w, h }; }
        int width() const { return w; }
        int height() const { return h; }

        void set_exclusive_mouse(bool exclusive) {
            this->relative_mode = exclusive;
            SDL_SetRelativeMouseMode(exclusive ? SDL_TRUE : SDL_FALSE);
        }

        bool exclusive_mouse() const {
            return this->relative_mode;
        }

        void toggle_text_input() const {
            if (SDL_IsTextInputActive())
                SDL_StopTextInput();
            else
                SDL_StartTextInput();
        }

        bool text_input_active() const {
            return SDL_IsTextInputActive() == SDL_TRUE;
        }

        template<typename TScene, typename... TArgs, typename = std::enable_if_t<std::is_base_of<scene::Scene, TScene>::value>>
        void set_scene(TArgs&&... args) {
            this->scene.reset();
            this->scene = std::make_unique<TScene>(*this, std::forward<TArgs>(args)...);
        }

        void set_scene(std::unique_ptr<scene::Scene> scene) {
            this->scene.reset();
            this->scene = std::move(scene);
        }
        
        friend net::NetworkClient;
        net::NetworkClient net;
        std::unique_ptr<gl::ShaderManager> shaders; // uniqe_ptr because GL context needs to be created before the shaders can be compiled and loaded.
        draw::SpriteManager sprites;
        sound::SoundManager sound;
        util::TaskScheduler tasks;
        draw::FontManager fonts;

        // Input state
        struct {
            int numkeys = 0;
            const Uint8 *keys = nullptr;
            SDL_Keymod mods = KMOD_NONE;
        } keyboard;
        struct {
            int x = 0, y = 0, dx = 0, dy = 0;
            Uint32 state = 0, dstate = 0;
        } mouse;

        std::string input_buffer;

        bool quit = false;
        double time = 0.0;

        std::unique_ptr<scene::Scene> scene; // very bad idea??
    private:
        void draw() const;
        void update(double dt);
        void update_fps();

        void poll_events();
        void handle_key_press(const SDL_Event &event);
        void handle_window_event(const SDL_Event &event);

        int w, h;
        struct {
            double last_update = 0.0;
            int frames = 0;
        } fps_counter;

        SDL_Window *window;
        SDL_GLContext context;
        std::string window_title;

        bool relative_mode{ false };
    };
}
