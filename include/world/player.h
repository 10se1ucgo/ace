#pragma once
#include "glm/glm.hpp"

#include "world/world.h"
#include "draw/map.h"
#include "draw/kv6.h"
#include "weapon.h"

namespace ace { 
    namespace scene {
        class GameScene;
        struct Team;
    }

    namespace world {
    struct Entity;

    struct AcePlayer {
        explicit AcePlayer(scene::GameScene &scene);

        scene::GameScene &scene;
        bool mf = false, mb = false, ml = false, mr = false;
        bool jump = false, crouch = false, sneak = false, sprint = false;
        bool primary_fire = false, secondary_fire = false, weapon_equipped = false;
        bool airborne = false, wade = false;
        bool alive = true;
        double lastclimb = 0.0;
        glm::vec3 p{ 0 }, e{ 0 }, v{ 0 }, f{ 1, 0, 0 }, s{ 0, 1, 0 }, h{ 0, 0, 1 };

    protected:
        long fixed_update(double dt);
        void boxclipmove(double dt);
        void reposition(double dt);

        long update_dead(double dt);

        void set_orientation(float x, float y, float z);
        bool try_uncrouch();
    };

    struct DrawPlayer : AcePlayer {
        explicit DrawPlayer(scene::GameScene& scene, bool local_player = false);

        void update(double dt);
        long fixed_update(double dt);
        void draw();

        void set_position(glm::vec3 position) {
            this->p = this->e = position;
        }
        void set_orientation(glm::vec3 orientation);

        bool set_walk(bool mf, bool mb, bool ml, bool mr);
        bool set_animation(bool jump, bool crouch, bool sneak, bool sprint);
        bool set_fire(bool primary, bool secondary);
        void set_crouch(bool value);
        void set_sprint(bool value);

        void set_tool(net::TOOL tool);
        void set_weapon(net::WEAPON weapon);
        void set_color(glm::u8vec3 color);

        void set_team(net::TEAM team);
        void set_team(scene::Team &team);
        scene::Team &team(bool other = false) const;

        void set_alive(bool value);
        void restock(bool primary = false);

        void play_sound(const std::string &name, int volume = 100) const;

        net::HIT test_ray(glm::vec3 origin, glm::vec3 dir, glm::vec3 *hit) const {
            if (this->mdl_head.sprhitscan(origin, dir, hit)) {
                return net::HIT::HEAD;
            }
            if (this->mdl_torso.sprhitscan(origin, dir, hit)) {
                return net::HIT::TORSO;
            }
            if (this->mdl_legl.sprhitscan(origin, dir, hit)) {
                return net::HIT::LEGS;
            }
            if (this->mdl_legr.sprhitscan(origin, dir, hit)) {
                return net::HIT::LEGS;
            }
            return net::HIT::INVALID;
        }

        uint8_t pid{}, health{};
        net::WEAPON equipped_weapon{net::WEAPON::INVALID};
        net::TOOL held_tool{net::TOOL::INVALID};
        
        uint32_t score{};
        glm::u8vec3 color{112, 112, 112};
        std::string name;

        draw::KV6 mdl_head, mdl_torso, mdl_legr, mdl_legl, mdl_arms, mdl_dead;
        draw::KV6Mesh &msh_intel;
        bool local_player{false};

        glm::vec3 draw_forward{1, 0, 0}, draw_right{0, 0, 1}, draw_up{0, 1, 0};

        BlockTool blocks;
        SpadeTool spade;
        GrenadeTool grenades;
        std::unique_ptr<Weapon> weapon_obj;

//        Entity *ent;

        Tool *get_tool(net::TOOL tool = net::TOOL::INVALID);

        double switch_time{}, next_footstep{};
    private:
        net::TEAM _team{ net::TEAM::SPECTATOR };
        void transform();
    };

    // TODO: This is a crap load of code duplication.
    // Eventually, DrawPlayer will be renamed to something less arbitrary and this will act as the drawable component
    // Similar to the transformation of AceMap => DrawMap to AceMap (map data) => World (building, destroying, falling, etc logic) + MapRenderer
    // AcePlayer => DrawPlayer to AcePlayer (world movement) => Player (tools, weapons, multiplayer data, etc logic) + PlayerModel 
    struct PlayerModel {
        PlayerModel(scene::GameScene &scene);
        void draw(glm::vec3 color, bool local = false);
        void transform(glm::vec3 pos, float yaw, float pitch, bool crouch = false);

        scene::GameScene &scene;
        draw::KV6 mdl_head, mdl_torso, mdl_legr, mdl_legl, mdl_arms, mdl_dead;
    };
}}