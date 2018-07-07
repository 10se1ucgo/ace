#pragma once
#include <utility>

#include "glm/glm.hpp"

#include "scene/scene.h"
#include "scene/hud.h"

#include "world/world.h"
#include "world/player.h"
#include "world/entity.h"

#include "draw/billboard.h"
#include "draw/map.h"
#include "gl/shader.h"

#include "camera.h"
#include "kv6.h"
#include "net/net.h"
#include "util/event.h"
#include "draw/debug.h"


namespace ace { namespace scene {
    struct Team {
        Team() : Team("Neutral", {255, 255, 255}, net::TEAM::NEUTRAL) {}
        Team(std::string name, glm::ivec3 color, net::TEAM id) :
            name(std::move(name)),
            color(color),
            float_color(glm::vec3(color) / 255.f),
            id(id) {
            
        }
        void update_players(GameScene &scene);

        std::string name;
        glm::ivec3 color;
        glm::vec3 float_color;
        net::TEAM id;
        int score{}, max_score{};

        std::vector<world::DrawPlayer *> players;
    };

    // i dont think this needs padding to be std140 compliant (we'll see :^))
#pragma pack(push, 1)
    struct SceneUniforms {
        glm::mat4 view, proj, pv;
        glm::vec3 cam_forward; float ___pad1;
        glm::vec3 cam_right; float ___pad2;
        glm::vec3 cam_up; float ___pad3;
        glm::vec3 fog_color; float ___pad4;
        glm::vec3 light_pos; float ___pad5;
    };
#pragma pack(pop)

    class GameScene final : public Scene {
    public:
        GameScene(GameClient &client, const net::StateData &state_data, std::string ply_name="Deuce", uint8_t *buf=nullptr);
        // GameScene(GameClient& client, const std::string& map_name);
        ~GameScene();

        void start();
        void draw() override;
        void update(double dt) override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_motion(int x, int y, int dx, int dy) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_window_resize(int ow, int oh) override;
        
        void on_net_event(net::NetState event) override;
        void on_packet(net::PACKET type, std::unique_ptr<net::Loader> ploader) override;

        bool on_text_typing(const std::string &text) override;
        void on_text_finished(bool cancelled) override;

        bool build_point(int x, int y, int z, glm::u8vec3 color, bool s2c = false);
        bool destroy_point(int x, int y, int z, net::ACTION type=net::ACTION::DESTROY, bool s2c = false);
        bool damage_point(int x, int y, int z, uint8_t damage, Face f=Face::INVALID, bool destroy = true);

        void set_zoom(bool zoom);
        void set_fog_color(glm::vec3 color);

        void send_block_action(int x, int y, int z, net::ACTION type = net::ACTION::BUILD) const;
        void send_block_line(glm::ivec3 p1, glm::ivec3 p2) const;
        void send_position_update() const;
        void send_orientation_update() const;
        void send_input_update() const;
        void send_grenade(float fuse) const;
        void send_team_change(net::TEAM new_team) const;
        void send_weapon_change(net::WEAPON new_weapon) const;
        void send_this_player(net::TEAM team, net::WEAPON weapon) const;

        void respawn_entities();

        template<typename TObj, typename... TArgs, typename = std::enable_if_t<std::is_base_of<world::WorldObject, TObj>::value>>
        world::WorldObject *create_object(TArgs&&... args) {
            queued_objects.emplace_back(std::make_unique<TObj>(*this, std::forward<TArgs>(args)...));
            return queued_objects.back().get();
        }

        gl::ShaderManager &shaders;
        gl::experimental::ubo<SceneUniforms> uniforms;
        draw::BillboardManager billboards;
        draw::DebugDraw debug;
        KV6Manager models; // todo move this to GameClient, no point re-loading every single KV6 every new map.
        Camera cam;
        draw::DrawMap map;
        HUD hud;
        world::DrawPlayer *ply{nullptr};
        net::StateData state_data;

        bool thirdperson{};

        std::unordered_map<int, std::unique_ptr<world::DrawPlayer>> players;
        std::unordered_map<int, glm::u8vec3> block_colors;
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
