#pragma once
#include "glm/glm.hpp"

#include "world/world.h"
#include "draw/map.h"
#include "kv6.h"
#include "weapon.h"

namespace ace { 
    namespace scene {
        class GameScene;
    }

    namespace world {
    struct Entity;

    struct AcePlayer {
        explicit AcePlayer(scene::GameScene &scene);
        long update(double dt);
        long update_dead(double dt);
        void set_orientation(float x, float y, float z);

        scene::GameScene &scene;
        bool mf, mb, ml, mr, jump, crouch, sneak, sprint, primary_fire, secondary_fire, airborne, wade, alive, weapon_equipped;
        double lastclimb;
        glm::vec3 p, e, v, f, s, h;

        bool try_uncrouch();
    private:
        void boxclipmove(double dt);
        void reposition(double dt);
    };

    struct DrawPlayer : AcePlayer {
        explicit DrawPlayer(scene::GameScene& scene, bool local_player = false);

        long update(double dt);
        void draw();

        void set_position(float x, float y, float z);
        void set_orientation(float x, float y, float z);

        bool set_walk(bool mf, bool mb, bool ml, bool mr);
        bool set_animation(bool jump, bool crouch, bool sneak, bool sprint);
        bool set_fire(bool primary, bool secondary);
        void set_crouch(bool value);
        void set_sprint(bool value);

        void set_tool(net::TOOL tool);
        void set_weapon(net::WEAPON weapon);
        void set_color(glm::u8vec3 color);
        void set_alive(bool value);
        void restock(bool primary = false);

        void play_sound(const std::string &name, int volume=100) const;

        

        uint8_t pid{}, health{};
        net::WEAPON weapon{net::WEAPON::INVALID};
        net::TOOL tool{net::TOOL::INVALID};
        net::TEAM team{net::TEAM::SPECTATOR};
        uint32_t kills{};
        glm::u8vec3 color{112, 112, 112};
        std::string name;

        KV6 mdl_head, mdl_torso, mdl_legr, mdl_legl, mdl_arms, mdl_dead;
        bool local_player{false};

        glm::vec3 draw_forward, draw_right;

        BlockTool blocks;
        SpadeTool spade;
        GrenadeTool grenades;
        std::unique_ptr<Weapon> weapon_obj;

//        Entity *ent;

        Tool *get_tool(net::TOOL tool = net::TOOL::INVALID);

        double switch_time, next_footstep{};
    private:
        void transform();
    };
}}