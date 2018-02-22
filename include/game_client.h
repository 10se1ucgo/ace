#pragma once
#include <string>

#include "SDL.h"
#include "glad/glad.h"
#include "fmt/format.h"
#include "sound/sound.h"
#include "util/event.h"
#include "draw/font.h"

namespace net {
    struct NetworkClient;
}

namespace ace {
    namespace scene {
        struct Scene;
    }

    enum class WINDOW {
        WINDOWED,
        FULLSCREEN,
        FULLSCREEN_BORDERLESS
    };

    class GameClient {
    public:
        GameClient(const std::string &caption, int w = 800, int h = 600, WINDOW style = WINDOW::WINDOWED);
        ~GameClient();
        ACE_NO_COPY_MOVE(GameClient)

        void run();

        void set_title(const std::string &caption) const {
            SDL_SetWindowTitle(this->window, caption.c_str());
        }
        std::pair<int, int> size() const { return { w, h }; }
        int width() const { return w; }
        int height() const { return h; }

        void set_exclusive_mouse(bool exclusive) {
            SDL_SetRelativeMouseMode(exclusive ? SDL_TRUE : SDL_FALSE);
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

        template<typename TScene, typename... TArgs>
        void set_scene(TArgs&&... args) {
            static_assert(std::is_base_of<scene::Scene, TScene>::value, "scene class must derive from testogl::Scene");
            this->scene.reset();
            this->scene = std::make_unique<TScene>(*this, std::forward<TArgs>(args)...);
        }
        

        struct {
            int numkeys = 0;
            const Uint8 *keys = nullptr;
            SDL_Keymod mods = KMOD_NONE;
        } keyboard;

        struct {
            int x = 0, y = 0, dx = 0, dy = 0;
            Uint32 state = 0, dstate = 0;
        } mouse;

        std::unique_ptr<net::NetworkClient> net;
        sound::SoundManager sound;
        util::TaskScheduler tasks;
        draw::FontManager fonts;

        bool quit = false;
        double time = 0.0;
        std::string input_buffer;
    private:
        int w, h;
        struct {
            double last_update = 0.0;
            int frames = 0;
        } fps_counter;

        SDL_Window *window;
        SDL_GLContext context;

        std::unique_ptr<scene::Scene> scene;
        friend net::NetworkClient;
        
        void draw() const;
        void update(double dt);
        void update_fps();

        void poll_events();
        void handle_key_press(const SDL_Event &event);
        void handle_window_event(const SDL_Event &event);
    };
}
