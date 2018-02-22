#include "scene/game.h"

#include "world/falling_blocks.h"

#include <SDL_image.h>
#include "util/except.h"
#include <glm/gtc/matrix_access.hpp>
#include "world/debris.h"
#include "world/grenade.h"

namespace ace { namespace scene {
    GameScene::GameScene(GameClient &client, const net::StateData &state_data, const std::vector<net::ExistingPlayer> &players, uint8_t *buf) :
        Scene(client),
        cam(*this, { 256, -50.f + 1, 256 }, { 0, 0, 1 }),
        map(*this, buf),
        hud(*this),
        state_data(state_data),
        teams({ {net::TEAM::TEAM1, Team(state_data.team1_name, state_data.team1_color)},
                {net::TEAM::TEAM2, Team(state_data.team2_name, state_data.team2_color)} }),
        pd_upd(this->client.tasks.call_every(1, false, &GameScene::send_position_update, this)),
        od_upd(this->client.tasks.call_every(1/30.f, false, &GameScene::send_orientation_update, this)) {

        // pyspades has a dumb system where sending more
        // than one PositionData packet every 0.7 seconds will cause you to rubberband
        // `if current_time - last_update < 0.7: rubberband()`
        // If anything, that < really should be a > but oh well.
        


        this->set_fog_color(glm::vec3(state_data.fog_color) / 255.f);

        this->shaders.model.bind();
        this->shaders.model.uniform("light_pos", glm::normalize(glm::vec3{ -0.16, 0.8, 0.56 }));

        glCullFace(GL_FRONT);
        glEnable(GL_MULTISAMPLE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        fmt::print("{} {} {}\n", state_data.pid, state_data.team1_name, state_data.team2_name);

        
        for(auto &ply : players) {
            auto *p = this->get_ply(ply.pid);
            p->pid = ply.pid;
            p->team = ply.team;
            p->name = ply.name;
            p->set_weapon(ply.weapon);
            p->set_tool(ply.tool);
            p->set_color(ply.color);
            p->kills = ply.kills;
        }

        if(state_data.mode == 0) {
            auto &mode = state_data.state.ctf;
            // uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier
            entities.emplace(uint8_t(net::OBJECT::BLUE_BASE), std::make_unique<world::CommandPost>(*this, 0, mode.team1_base, net::TEAM::TEAM1, 255));
            entities.emplace(uint8_t(net::OBJECT::GREEN_BASE), std::make_unique<world::CommandPost>(*this, 0, mode.team2_base, net::TEAM::TEAM2, 255));
            entities.emplace(uint8_t(net::OBJECT::BLUE_FLAG), std::make_unique<world::Flag>(*this, 0, mode.team1_flag, net::TEAM::TEAM1, mode.team1_carrier));
            entities.emplace(uint8_t(net::OBJECT::GREEN_FLAG), std::make_unique<world::Flag>(*this, 0, mode.team2_flag, net::TEAM::TEAM2, mode.team2_carrier));
        }

        this->ply = this->get_ply(state_data.pid, true, true);
        this->ply->set_position(256, 256, 0);


        this->client.sound.play("intro.wav", {}, 100, true);

        cam.set_projection(75.0f, client.width(), client.height(), 0.1f, 128.f);
    }

    GameScene::~GameScene() {
        this->client.tasks.remove_loop(pd_upd);
        this->client.tasks.remove_loop(od_upd);
        fmt::print("~GameScene()\n");
    }

    void GameScene::draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3d
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        shaders.map.bind();
        shaders.map.uniform("mvp", cam.matrix());
        shaders.map.uniform("alpha", 1.0f);
        shaders.map.uniform("replacement_color", glm::vec3(0.f));
        map.draw({ this->cam.position.x, this->cam.position.z, -this->cam.position.y }, shaders.map);


        shaders.model.bind();
        for (auto &kv : players) {
            auto p1 = vox2draw(kv.second->p - 1.f);
            auto p2 = vox2draw(kv.second->p + 1.f);
            if(this->cam.box_in_frustum(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z) && !kv.second->local_player)
                kv.second->draw();
        }

        for (auto &kv : entities) {
            kv.second->draw();
        }

        for (const auto &obj : objects) {
            obj->draw();
        }

        this->shaders.billboard.bind();
        this->shaders.billboard.uniform("cam_right", glm::vec3(glm::row(this->cam.view, 0)));
        this->shaders.billboard.uniform("cam_up", glm::vec3(glm::row(this->cam.view, 1)));
        this->billboards.draw(this->cam.matrix(), this->shaders.billboard);


        if (!this->thirdperson)
            glClear(GL_DEPTH_BUFFER_BIT);
        this->shaders.model.bind();
        this->ply->draw();

        hud.draw();
    }

    void GameScene::update(double dt) {
        Scene::update(dt);

        cam.update(dt);
        if (!this->thirdperson)
            this->ply->set_orientation(this->cam.forward.x, this->cam.forward.z, -this->cam.forward.y);
        for (auto &kv : players) {
            kv.second->update(dt);
        }
        cam.update();
        client.sound.set_listener(this->cam.position, this->cam.forward, glm::vec3(glm::row(this->cam.view, 1)));

        map.update(dt);

        for (auto &kv : entities) {
            kv.second->update(dt);
        }

        while(!queued_objects.empty()) {
            auto it = queued_objects.begin();
            this->objects.emplace_back(std::move(*it));
            queued_objects.erase(it);
        }
        for (auto i = objects.begin(); i != objects.end();) {
            if ((*i)->update(dt)) {
                i = objects.erase(i);
            } else {
                ++i;
            }
        }

        hud.update(dt);
    }

    void GameScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        hud.on_key(scancode, modifiers, pressed);

        if (pressed) {
            net::WEAPON wep = net::WEAPON::INVALID;

            switch(scancode) {
            case SDL_SCANCODE_1: this->ply->set_tool(net::TOOL::SPADE); break;
            case SDL_SCANCODE_2: this->ply->set_tool(net::TOOL::BLOCK); break;
            case SDL_SCANCODE_3: this->ply->set_tool(net::TOOL::WEAPON); break;
            case SDL_SCANCODE_4: this->ply->set_tool(net::TOOL::GRENADE); break;
            case SDL_SCANCODE_5: wep = net::WEAPON::SEMI; break;
            case SDL_SCANCODE_6: wep = net::WEAPON::SMG; break;
            case SDL_SCANCODE_7: wep = net::WEAPON::SHOTGUN; break;
            case SDL_SCANCODE_R: this->ply->get_tool()->reload(); break;
            case SDL_SCANCODE_F2: this->thirdperson = !this->thirdperson; break;
            case SDL_SCANCODE_F3: this->ply->alive = !this->ply->alive; break;
            default: break;
            }

            if(wep != net::WEAPON::INVALID) {
                net::ExistingPlayer x;
                x.name = "10se1ucgo";
                x.team = net::TEAM::TEAM1;
                x.weapon = wep;
                this->client.net->send_packet(net::PACKET::ExistingPlayer, x);
            }
        }
    };

    void GameScene::on_mouse_motion(int x, int y, int dx, int dy) {
        hud.on_mouse_motion(x, y, dx, dy);
    }

    void GameScene::on_mouse_button(int button, bool pressed) {
        hud.on_mouse_button(button, pressed);
    }

    void GameScene::on_window_resize(int ow, int oh) {
        hud.on_window_resize(ow, oh);
        glViewport(0, 0, this->client.width(), this->client.height());
        this->set_zoom(false);
    }

    void GameScene::on_packet(net::PACKET type, net::Loader *loader) {
        // this is bad I KNOW dont flame thanks :))
        switch(type) {
        case net::PACKET::CreatePlayer: {
            net::CreatePlayer *pkt = static_cast<net::CreatePlayer *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            ply->pid = pkt->pid;
            ply->set_weapon(pkt->weapon);
            ply->set_tool(net::TOOL::WEAPON);
            ply->team = pkt->team;
            ply->set_position(pkt->position.x, pkt->position.y, pkt->position.z);
            ply->name = pkt->name;
            ply->set_alive(true);
        } break;
        case net::PACKET::WorldUpdate: {
            net::WorldUpdate *pkt = static_cast<net::WorldUpdate *>(loader);
            for(int i = 0; i < 32; i++) {
                if (i == this->state_data.pid) continue;
                auto *p = this->get_ply(i, false);
                if (p == nullptr || !p->alive || p == this->ply) continue;

                const auto &wud = pkt->items.at(i);
                p->set_position(wud.first.x, wud.first.y, wud.first.z);
                p->set_orientation(wud.second.x, wud.second.y, wud.second.z);
            }
        } break;
        case net::PACKET::BlockAction: {
            net::BlockAction *pkt = static_cast<net::BlockAction *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            if(pkt->value == net::ACTION::BUILD) {
                this->build_point(pkt->position.x, pkt->position.y, pkt->position.z, ply ? ply->color : glm::vec3{255, 255, 255}, true);
                ply->get_tool(net::TOOL::BLOCK)->primary_ammo--;
            } else {
                this->destroy_point(pkt->position.x, pkt->position.y, pkt->position.z, pkt->value, true);
                if(pkt->value == net::ACTION::DESTROY)
                    ply->get_tool(net::TOOL::BLOCK)->primary_ammo++;
            }
        } break;
        case net::PACKET::BlockLine: {
            net::BlockLine *pkt = static_cast<net::BlockLine *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            std::vector<glm::ivec3> blocks = this->map.block_line(pkt->start, pkt->end);
            for(auto &block : blocks) {
                this->build_point(block.x, block.y, block.z, ply ? ply->color : glm::vec3{ 255, 255, 255 }, true);
            }
        } break;
        case net::PACKET::InputData: {
            net::InputData *pkt = static_cast<net::InputData *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->mf = pkt->up; ply->mb = pkt->down; ply->ml = pkt->left; ply->mr = pkt->right;
            ply->jump = pkt->jump; ply->sneak = pkt->sneak; ply->sprint = pkt->sprint;
            ply->set_crouch(pkt->crouch);
        } break;
        case net::PACKET::KillAction: {
            net::KillAction *pkt = static_cast<net::KillAction *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            auto *killer = this->get_ply(pkt->killer, false);
            if (ply == nullptr || killer == nullptr) break;
            ply->set_alive(false);
            fmt::print("{} killed {}. Respawning in {}\n", killer->name, ply->name, pkt->respawn_time);
            this->hud.add_killfeed_message(*killer, *ply, pkt->type);

            if(ply->pid == this->ply->pid) {
                this->hud.respawn_time = pkt->respawn_time;
            }
        } break;
        case net::PACKET::PositionData: {
            if (this->ply) {
                auto pos = static_cast<net::PositionData *>(loader)->position;
                this->ply->set_position(pos.x, pos.y, pos.z);
            }
        } break;
        case net::PACKET::WeaponInput: {
            net::WeaponInput *pkt = static_cast<net::WeaponInput *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->primary_fire = pkt->primary;
            ply->secondary_fire = pkt->secondary;
        } break;
        case net::PACKET::SetHP: {
            net::SetHP *pkt = static_cast<net::SetHP *>(loader);
            if (!this->ply) return;

            this->ply->health = pkt->hp;
            if (pkt->type != net::DAMAGE::FALL) {
                this->client.sound.play("hitplayer.wav", {}, 100, true);

                this->hud.set_hit(pkt->source);
            }
        }  break;
        case net::PACKET::GrenadePacket: {
            net::GrenadePacket *pkt = static_cast<net::GrenadePacket *>(loader);
            this->create_object<ace::world::Grenade>(pkt->position, pkt->velocity, pkt->fuse);
        } break;
        case net::PACKET::SetTool: {
            net::SetTool *pkt = static_cast<net::SetTool *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->set_tool(pkt->tool);
        } break;
        case net::PACKET::SetColor: {
            net::SetColor *pkt = static_cast<net::SetColor *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->set_color(pkt->color);
        } break;
        case net::PACKET::ChatMessage: {
            net::ChatMessage *pkt = static_cast<net::ChatMessage *>(loader);
            if(pkt->pid >= 32 || pkt->type == net::CHAT::SYSTEM) {
                this->hud.add_chat_message(fmt::format("[*]: {}", pkt->message), {1, 0, 0});
                return;
            }

            auto *ply = this->get_ply(pkt->pid, false);
            
            if (ply == nullptr) {
                fmt::print("chat missed from {}: {}\n", pkt->pid, pkt->message);
                break;
            }

            if(pkt->type == net::CHAT::ALL) {
                auto msg = fmt::format("{} ({}): {}", ply->name, teams[ply->team].name, pkt->message);
                this->hud.add_chat_message(msg, {1, 1, 1});
            } else {
                auto msg = fmt::format("{}: {}", ply->name, pkt->message);
                this->hud.add_chat_message(msg, teams[ply->team].float_color);
            }
        } break;
        case net::PACKET::MoveObject: {
            net::MoveObject *pkt = static_cast<net::MoveObject *>(loader);
            auto ent = this->get_ent(uint8_t(pkt->type));
            ent->set_team(pkt->state);
            ent->set_position(pkt->position);
        } break;
        case net::PACKET::PlayerLeft: {
            net::PlayerLeft *pkt = static_cast<net::PlayerLeft *>(loader);
            this->players.erase(pkt->pid);
        } break;
        case net::PACKET::TerritoryCapture: break;
        case net::PACKET::ProgressBar: break;
        case net::PACKET::IntelCapture: {
            net::IntelCapture *pkt = static_cast<net::IntelCapture *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if(ply != nullptr) {
                fmt::print("{} captured the {} team Intel!", ply->name, this->get_team(ply->team, true).name);
            } 
            if(pkt->winning) {
                this->client.sound.play("horn.wav", { 0,0,0 }, 100, true);
            }
        } break;
        case net::PACKET::IntelPickup: {
            net::IntelPickup *pkt = static_cast<net::IntelPickup *>(loader);

            auto *ply = this->get_ply(pkt->pid, false);
            if (ply != nullptr) {
                auto msg = fmt::format("{} has the {} Intel", ply->name, this->get_team(ply->team, true).name);
                this->hud.add_chat_message(msg, { 1, 0, 0 });
            }
            this->client.sound.play("pickup.wav", { 0, 0, 0 }, 100, true);
            auto *ent = this->get_ent(uint8_t(ply->team == net::TEAM::TEAM1 ? net::OBJECT::GREEN_FLAG : net::OBJECT::BLUE_FLAG));
            ent->set_carrier(ply->pid);
        } break;
        case net::PACKET::IntelDrop: {
            net::IntelDrop *pkt = static_cast<net::IntelDrop *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply != nullptr) {
                auto msg = fmt::format("{} has dropped the {} Intel", ply->name, this->get_team(ply->team, true).name);
                this->hud.add_chat_message(msg, { 1, 0, 0 });
            }
            auto *ent = this->get_ent(uint8_t(ply->team == net::TEAM::TEAM1 ? net::OBJECT::GREEN_FLAG : net::OBJECT::BLUE_FLAG));
            ent->set_carrier(-1);
            ent->set_position(pkt->pos);
        } break;
        case net::PACKET::Restock: {
//            net::Restock *pkt = static_cast<net::Restock *>(loader);
//            auto *ply = this->get_ply(pkt->pid, false);
            // why does Restock have a pid field if it's never used??
            if(this->ply) {
                this->ply->restock();
            }
        } break;
        case net::PACKET::FogColor: {
            this->set_fog_color(glm::vec3(static_cast<net::FogColor *>(loader)->color) / 255.f);
        } break;
        case net::PACKET::WeaponReload: {
            
            net::WeaponReload *pkt = static_cast<net::WeaponReload *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            fmt::print("PLY {} RELOADING\n", pkt->pid);
            if (!ply || !ply->weapon_obj) break;
            ply->weapon_obj->on_reload(pkt);
        } break;
        default: ;
        }
    }

    bool GameScene::on_text_typing(const std::string& text) {
        return this->hud.on_text_typing(text);
    }

    void GameScene::on_text_finished() {
        this->hud.on_text_finished();
    }

    bool GameScene::build_point(int x, int y, int z, const glm::ivec3& color, bool s2c) {
        if (s2c) {
            this->client.sound.play("build.wav", vox2draw(glm::vec3{ x, y, z } + 0.5f), 50);
            return map.build_point(x, y, z, color, s2c);
        }
        this->send_block_action(x, y, z, net::ACTION::BUILD);
        return false;
    }

    bool GameScene::destroy_point(int x, int y, int z, net::ACTION type, bool s2c) {
        if(!s2c) {
            this->send_block_action(x, y, z, type);
            return false;
        }

        std::vector<VXLBlock> v;
        bool ok = map.destroy_point(x, y, z, v);
        switch(type) {
        case net::ACTION::SPADE:
            ok |= map.destroy_point(x, y, z + 1, v);
            ok |= map.destroy_point(x, y, z - 1, v);
            break;
        case net::ACTION::GRENADE:
            for (int i = x - 1; i < x + 2; i++) {
                for (int j = y - 1; j < y + 2; j++) {
                    for(int k = z - 1; k < z + 2; k++) {
                        ok |= map.destroy_point(i, j, k, v);
                    }
                }
            }
            break;
        default:
            break;
        }
        if (!v.empty()) {
            this->create_object<world::FallingBlocks>(v);
//            objects.emplace_back(std::make_unique<world::FallingBlocks>(*this, v));
        }
        return ok;
    }

    bool GameScene::damage_point(int x, int y, int z, uint8_t damage, Face f, bool destroy) {
        if(f != Face::INVALID) {
            uint8_t a, r, g, b;
            unpack_color(this->map.get_color(x, y, z), &a, &r, &g, &b);
            this->create_object<world::DebrisGroup>(this->map.get_face(x, y, z, f), glm::vec3{ r, g, b }, 0.25f, 4);
        }

        if (damage && this->map.damage_point(x, y, z, damage) && destroy) {
            this->destroy_point(x, y, z);
            return true;
        }
        return false;
    }

    void GameScene::set_zoom(bool zoom) {
        cam.sensitivity = zoom ? 0.05f : 0.1f;
        cam.set_projection(zoom ? 37.5f : 75.0f, client.width(), client.height(), 0.1f, 128.f);
    }

    void GameScene::send_block_action(int x, int y, int z, net::ACTION type) {
        if (type == net::ACTION::GRENADE) return;

        net::BlockAction pkt;
        pkt.position = { x, y, z };
        pkt.value = type;
        this->client.net->send_packet(net::PACKET::BlockAction, pkt);
    }

    void GameScene::send_position_update() {
        if (!this->ply || !this->ply->alive) return;

        net::PositionData pd;
        pd.position = this->ply->p;
        this->client.net->send_packet(net::PACKET::PositionData, pd, ENET_PACKET_FLAG_UNSEQUENCED);
    }

    void GameScene::send_orientation_update() {
        if (!this->ply || !this->ply->alive) return;

        net::OrientationData od;
        od.orientation = this->ply->f;
        this->client.net->send_packet(net::PACKET::OrientationData, od, ENET_PACKET_FLAG_UNSEQUENCED);
    }

    void GameScene::send_input_update() {
        if (!this->ply || !this->ply->alive) return;

        net::InputData id;
        id.pid = this->ply->pid;
        id.up = this->ply->mf;
        id.down = this->ply->mb;
        id.left = this->ply->ml;
        id.right = this->ply->mr;
        id.crouch = this->ply->crouch;
        id.jump = this->ply->jump;
        id.sneak = this->ply->sneak;
        id.sprint = this->ply->sprint;
        this->client.net->send_packet(net::PACKET::InputData, id);

        net::WeaponInput wi;
        wi.primary = this->ply->primary_fire;
        wi.secondary = this->ply->secondary_fire;
        this->client.net->send_packet(net::PACKET::WeaponInput, wi);
    }

    void GameScene::send_grenade(float fuse) {
        net::GrenadePacket gp;
        gp.position = this->ply->p;
        gp.velocity = this->ply->f + this->ply->v;
        gp.fuse = fuse;
        this->client.net->send_packet(net::PACKET::GrenadePacket, gp);
    }

    void GameScene::set_fog_color(glm::vec3 color) {
        glClearColor(color.r, color.g, color.b, 1.0f);
        // todo: uniform buffer?
        shaders.map.bind();
        shaders.map.uniform("fog_color", color);
        shaders.model.bind();
        shaders.model.uniform("fog_color", color);
    }
}}
