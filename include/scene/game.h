#pragma once
#include <utility>
#include "glad/glad.h"
#include "glm/gtx/string_cast.hpp"

#include "scene.h"
#include "gl/shader.h"
#include "kv6.h"
#include "world/world.h"
#include "world/player.h"
#include "camera.h"
#include "draw/map.h"
#include "scene/hud.h"
#include "world/entity.h"
#include "net.h"
#include "draw/billboard.h"

namespace ace { namespace scene {
    struct Team {
        Team() : Team("Neutral", {255, 255, 255}, net::TEAM::NEUTRAL) {}
        Team(std::string name, glm::ivec3 color, net::TEAM id) :
            name(std::move(name)),
            color(color),
            float_color(glm::vec3(color) / 255.f),
            id(id),
            score(0), max_score(0) {
            
        }
        void update_players(GameScene &scene);

        std::string name;
        glm::ivec3 color;
        glm::vec3 float_color;
        net::TEAM id;
        int score{};
        int max_score{};

        std::vector<world::DrawPlayer *> players;
    };

    class GameScene final : public Scene {
    public:
        GameScene(GameClient &client, const net::StateData &state_data, std::string ply_name="Deuce", uint8_t *buf=nullptr);
        // GameScene(GameClient& client, const std::string& map_name);
        ~GameScene();

        void draw() override;
        void update(double dt) override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_window_resize(int ow, int oh) override;
        
        void on_packet(net::PACKET type, std::unique_ptr<net::Loader> loader) override;

        bool on_text_typing(const std::string &text) override;
        void on_text_finished() override;

        bool build_point(int x, int y, int z, const glm::ivec3& color, bool s2c=false);
        bool destroy_point(int x, int y, int z, net::ACTION type=net::ACTION::DESTROY, bool s2c = false);
        bool damage_point(int x, int y, int z, uint8_t damage, Face f=Face::INVALID, bool destroy = true);

        void set_zoom(bool zoom);
        void set_fog_color(glm::vec3 color);


        void send_block_action(int x, int y, int z, net::ACTION type = net::ACTION::BUILD);
        void send_position_update();
        void send_orientation_update();
        void send_input_update();
        void send_grenade(float fuse);
        void send_team_change(net::TEAM new_team);
        void send_weapon_change(net::WEAPON new_weapon);

        template<typename TObj, typename... TArgs, typename = std::enable_if_t<std::is_base_of<world::WorldObject, TObj>::value>>
        world::WorldObject *create_object(TArgs&&... args) {
            queued_objects.emplace_back(std::make_unique<TObj>(*this, std::forward<TArgs>(args)...));
            return queued_objects.back().get();
        }

        ShaderManager &shaders;
        draw::BillboardManager billboards;
        KV6Manager models; // todo move this to GameClient, no point re-loading every single KV6 every new map.
        Camera cam;
        draw::DrawMap map;
        HUD hud;
        world::DrawPlayer *ply;
        net::StateData state_data;

        bool thirdperson{};

        std::unordered_map<int, std::unique_ptr<world::DrawPlayer>> players;
        std::unordered_map<net::TEAM, Team> teams;
        std::unordered_map<uint8_t, std::unique_ptr<world::Entity>> entities;
        std::vector<std::unique_ptr<world::WorldObject>> objects;
        std::vector<std::unique_ptr<world::WorldObject>> queued_objects;

        world::DrawPlayer *get_ply(int pid, bool create=true, bool local_player=false) {
            auto ply = players.find(pid);
            world::DrawPlayer *ptr;
            if(ply != players.end()) {
                ptr = ply->second.get();
            } else if(create) {
                auto x = std::make_unique<world::DrawPlayer>(*this, local_player);
                ptr = players.insert({ pid, std::move(x) }).first->second.get();
            } else {
                return nullptr;
            }
            ptr->pid = pid;
            return ptr;
        }

        world::Entity *get_ent(int id) {
            auto ent = entities.find(id);
            if (ent != entities.end())
                return ent->second.get();
            return nullptr;
        }

        Team &get_team(net::TEAM id, bool other = false) {
            if (other)
                id = id == net::TEAM::TEAM1 ? net::TEAM::TEAM2 : net::TEAM::TEAM1;
            return this->teams[id];
        }

        util::TaskScheduler::loop_type pd_upd, od_upd;
        std::string ply_name;
    };
}}
