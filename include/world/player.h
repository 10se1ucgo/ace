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


        scene::GameScene &scene;
        bool mf, mb, ml, mr, jump, crouch, sneak, sprint, primary_fire, secondary_fire, airborne, wade, alive, weapon_equipped;
        double lastclimb;
        glm::vec3 p, e, v, f, s, h;

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
        void set_alive(bool value);
        void restock(bool primary = false);

        void play_sound(const std::string &name, int volume = 100) const;

        uint8_t pid{}, health{};
        net::WEAPON equipped_weapon{net::WEAPON::INVALID};
        net::TOOL held_tool{net::TOOL::INVALID};
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

    // TODO: This is a crap load of code duplication.
    // Eventually, DrawPlayer will be renamed to something less arbritrary and this will act as the drawable component
    struct PlayerModel {
        PlayerModel(scene::GameScene &scene);
        void draw(glm::vec3 color, bool local = false);
        void transform(glm::vec3 pos, float yaw, float pitch, bool crouch = false);

        scene::GameScene &scene;
        KV6 mdl_head, mdl_torso, mdl_legr, mdl_legl, mdl_arms, mdl_dead;
    };
}}