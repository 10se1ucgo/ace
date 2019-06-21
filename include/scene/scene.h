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

        virtual void start() { }

        virtual void update(double dt) {
            this->time += dt;
            this->ms_time = this->time * 1000;
        }
        virtual void fixed_update(double dt) {
            this->fixed_time += dt;
            this->ms_fixed_time = this->fixed_time * 1000;
        }
        virtual void draw() { }

        virtual void on_key(SDL_Scancode scancode, int modifiers, bool pressed) { }
        virtual void on_mouse_button(int button, bool pressed) { }
        virtual void on_mouse_motion(int x, int y, int dx, int dy) { }
        virtual void on_window_resize(int ow, int oh) { }
        virtual void on_mouse_scroll(int vertical, int horizontal) { }

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

#pragma pack(push, 1)
    // i cant design anything 
    // also idk what to name this
    struct Uniforms3D {
        glm::mat4 view, proj, pv;
        glm::vec3 cam_pos; float ___pad0;
        glm::vec3 cam_forward; float ___pad1;
        glm::vec3 cam_right; float ___pad2;
        glm::vec3 cam_up; float ___pad3;
        glm::vec3 fog_color; float ___pad4;
        glm::vec3 light_pos;
        // based on my understanding of std140 this should be aligned ok??
        // i should prob use one of the padding floats instead but that would look ugly!!
        float fog_start;
        float fog_end;
    };
#pragma pack(pop)
}}
