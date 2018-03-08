#pragma once
#include "game_client.h"

#include "net/packet.h"

namespace ace { namespace scene {
    struct Scene {
        Scene(ace::GameClient &client) : time(0.0), ms_time(0), client(client) {
        }
        virtual ~Scene() = default;

        virtual void update(double dt) { this->time += dt; ms_time = this->time * 1000; }
        virtual void draw() { }

        virtual void on_key(SDL_Scancode scancode, int modifiers, bool pressed) { };
        virtual void on_mouse_button(int button, bool pressed) { }
        virtual void on_mouse_motion(int x, int y, int dx, int dy) { }
        virtual void on_window_resize(int ow, int oh) { }

        virtual bool on_text_typing(const std::string &text) { return true; }
        virtual void on_text_finished() { }

        virtual void on_packet(net::PACKET type, std::unique_ptr<net::Loader> packet) { fmt::print("UNHANDLED PACKET {}\n", type); };

        double time;
        uint64_t ms_time;
        ace::GameClient &client;
    };

    class GameScene;
    class LoadingScene;
}}
