#pragma once
#include "SDL.h"
#include "net/net.h"

namespace ace { 
    class GameClient;
    
    namespace scene {
    struct Scene {
        Scene(ace::GameClient &client) : client(client) {
        }
        virtual ~Scene() = default;

        virtual void update(double dt) {
            this->time += dt;
            this->ms_time = this->time * 1000;
        }
        virtual void fixed_update(double dt) {
            this->fixed_time += dt;
            this->ms_fixed_time = this->fixed_time * 1000;
        }
        virtual void draw() { }

        virtual void on_key(SDL_Scancode scancode, int modifiers, bool pressed) { };
        virtual void on_mouse_button(int button, bool pressed) { }
        virtual void on_mouse_motion(int x, int y, int dx, int dy) { }
        virtual void on_window_resize(int ow, int oh) { }

        virtual bool on_text_typing(const std::string &text) { return true; }
        virtual void on_text_finished(bool cancelled) { }

        virtual void on_net_event(net::NetState event) { }
        virtual void on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) { fmt::print("UNHANDLED PACKET {}\n", type); };

        double time = 0.0, fixed_time = 0.0;
        uint64_t ms_time = 0, ms_fixed_time = 0;
        ace::GameClient &client;
    };

    class GameScene;
    class LoadingScene;
}}
