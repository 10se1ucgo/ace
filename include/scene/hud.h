#pragma once
#include "draw/sprite.h"
#include "scene/scene.h"
#include <deque>
#include "world/player.h"

namespace ace { namespace scene {
    struct Message {
        std::string message;
        glm::vec3 color;
    };

    struct HUD;

    struct MapDisplay {
        MapDisplay(HUD &hud);

        void update(double dt);
        void draw();

        HUD &hud;
        draw::Sprite big;
        draw::SpriteGroup *marker;
        bool visible{false};
    };

    struct HUD {
        HUD(GameScene &s);

        // TODO: instead of basically copying the ace::scene::Scene class
        // HUD and Scene should inherit from a base that declares these.
        void update(double dt);
        void draw();

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed);
        void on_mouse_button(int button, bool pressed);
        void on_mouse_motion(int x, int y, int dx, int dy);

        bool on_text_typing(const std::string &text);
        void on_text_finished();

        void on_window_resize(int ow, int oh);

        void add_chat_message(const std::string &message, glm::vec3 color);
        void add_killfeed_message(world::DrawPlayer &killer, world::DrawPlayer &victim, net::KILL kill_type);
        
        void set_hit(glm::vec3 source);

        void update_weapon(const std::string &sight);
        void update_tool(const std::string &ammo_icon);

        GameScene &scene;

        draw::SpriteManager sprites;

        draw::Sprite reticle, pal, palret, hit_indicator, weapon_sight, ammo_icon;
        glm::mat4 projection;

        MapDisplay map_display;
        

        int color_index = 0;
        float respawn_time = 0.0f;

        glm::vec3 last_hit;
    private:
        void update_color();
        void draw_chat();

        net::CHAT cur_chat_type{ net::CHAT::INVALID };
        draw::Font *sys48, *sys8, *sys15;
        std::deque<Message> chat_messages, killfeed;

        friend struct MapDisplay;
    };
}}
