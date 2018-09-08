#pragma once
#include <deque>

#include "scene/scene.h"
#include "draw/sprite.h"
#include "draw/font.h"
#include "world/player.h"

namespace ace { namespace scene {
    struct Message {
        std::string message;
        glm::vec3 color;
        float time{10.f};
    };

    struct HUD;

    struct MapDisplay : MapListener {
        explicit MapDisplay(HUD &hud);

        void update(double dt);
        void draw();

        HUD &hud;
        draw::SpriteGroup *marker, *map;
        draw::Sprite big, mini;
        bool big_open{false};
    private:
        draw::SpriteGroup *get_overview() const;
        void draw_map_grid(glm::vec2 offset) const;

        void on_block_changed(int x, int y, int z, AceMap &map) override;
    };

    struct WeaponChangeMenu {
        // Eventually, this will become an actual UI menu with widgets, similar to retail Ace of Spades.
        explicit WeaponChangeMenu(HUD &hud);

        void update(double dt);
        void draw_3d();

        HUD &hud;
        KV6 semi, smg, shotgun;
    };

    struct TeamChangeMenu {
        explicit TeamChangeMenu(HUD &hud);

        void update(double dt);
        void draw_3d();

        HUD &hud;
        world::PlayerModel p1, p2;
    };

    struct HUD {
        HUD(GameScene &s);

        void update(double dt);
        void draw();
        void draw_3d();

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed);
        void on_mouse_button(int button, bool pressed);
        void on_mouse_motion(int x, int y, int dx, int dy);

        bool on_text_typing(const std::string &text);
        void on_text_finished(bool cancelled);

        void on_window_resize(int ow, int oh);

        void add_chat_message(std::string message, glm::vec3 color);
        void add_killfeed_message(const world::DrawPlayer &killer, const world::DrawPlayer &victim, net::KILL kill_type);
        void set_big_message(std::string message);
        
        void set_hit(glm::vec3 source);

        void update_weapon(const std::string &sight);
        void update_tool(const std::string &ammo_icon);

        GameScene &scene;

        draw::SpriteManager &sprites;

        draw::Sprite reticle, pal, palret, hit_indicator, weapon_sight, ammo_icon;
        glm::mat4 projection;

        MapDisplay map_display;
        WeaponChangeMenu wep_change_menu;
        TeamChangeMenu team_change_menu;

        int color_index = 0;
        float respawn_time = 0.0f, big_message_time = 0.0f;

        glm::vec3 last_hit;
    private:
        enum class Change {
            None,
            Team,
            Weapon,
        } change_state = Change::Team;

        enum class FirstJoin {
            No,
            Yes,
            YesTeam1,
            YesTeam2
        } first_join = FirstJoin::Yes;

        bool is_exitting = false;

        void add_killfeed_message(std::string message, glm::vec3 color);

        void change_team(SDL_Scancode scancode);
        void change_weapon(SDL_Scancode scancode);
        void update_color(SDL_Scancode key);

        void draw_chat();
        void draw_killfeed();
        void draw_scoreboard();

        net::CHAT cur_chat_type{ net::CHAT::INVALID };
        draw::Font *sys48, *sys13, *sys15, *sys16;
        std::deque<Message> chat_messages, killfeed;
        std::string big_message;

        friend MapDisplay;
        friend WeaponChangeMenu;
        friend TeamChangeMenu;
    };
}}
