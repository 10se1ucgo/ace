#include "scene/game.h"

#include "SDL.h"

#include "game_client.h"
#include "world/debris.h"
#include "world/grenade.h"
#include "world/falling_blocks.h"
#include "util/except.h"
#include "scene/loading.h"

using namespace ace::gl::literals;
using namespace std::chrono_literals;

namespace ace { namespace scene {
    void Team::update_players(GameScene &scene) {
        this->players.clear();
        for (const auto &kv : scene.players) {
            auto *ply = kv.second.get();
            if(ply->team == this->id) {
                this->players.push_back(ply);
            }
        }
        std::sort(this->players.begin(), this->players.end(), [](const auto *a, const auto *b) { return a->kills > b->kills; });
    }

    GameScene::GameScene(GameClient &client, const net::StateData &state_data, std::string ply_name, uint8_t *buf) :
        Scene(client),
        shaders(*client.shaders),
        uniforms(this->shaders.create_ubo<Uniforms3D>("SceneUniforms")),
        cam(*this, { 256, 0, 256 }, { 0, -1, 0 }),
        world(*this, buf),
        hud(*this),
        state_data(state_data),
        teams({ {net::TEAM::TEAM1, Team(state_data.team1_name, state_data.team1_color, net::TEAM::TEAM1)},
                {net::TEAM::TEAM2, Team(state_data.team2_name, state_data.team2_color, net::TEAM::TEAM2)} }),
        ply_name(std::move(ply_name)),
        thirdperson(this->cam.thirdperson) {
        // pyspades has a dumb system where sending more
        // than one PositionData packet every 0.7 seconds will cause you to rubberband
        // `if current_time - last_update < 0.7: rubberband()`
        // If anything, that < really should be a > but oh well.

        this->set_fog_color(glm::vec3(state_data.fog_color) / 255.f);

        this->respawn_entities();

        this->set_zoom(false);
    }

    GameScene::~GameScene() {
        this->client.set_exclusive_mouse(false);
        fmt::print("~GameScene()\n");
    }

    void GameScene::start() {
        this->client.sound.play_local("intro.wav");
        this->client.set_exclusive_mouse(true);

        this->client.tasks.schedule(1.0, [this](util::Task &t) {
            this->send_position_update(); t.keep_going();
        });
        this->client.tasks.schedule(1.0 / 30, [this](util::Task &t) {
            this->send_orientation_update(); t.keep_going();
        });
    }

    void GameScene::draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 3d
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        

        this->uniforms.upload();

        this->shaders.map.bind();
        this->shaders.map.uniforms("model"_u = glm::mat4(1.0), "alpha"_u = 1.0f, "replacement_color"_u = glm::vec3(0.f));
        this->world.draw();

        this->shaders.model.bind();
        for (auto &kv : this->players) {
            auto p1 = vox2draw(kv.second->p - 1.f);
            auto p2 = vox2draw(kv.second->p + 1.f);
            if(this->cam.box_in_frustum(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z) && !kv.second->local_player)
                kv.second->draw();
        }

        for (auto &kv : entities) {
            kv.second->draw();
        }

#ifndef NDEBUG 
        if(this->ply) 
            this->debug.draw_ray(vox2draw(this->ply->e), this->ply->draw_forward * 25.f, this->get_team(this->ply->team).float_color);
#endif

        this->shaders.billboard.bind();
        this->billboards.flush(this->shaders.billboard);

        this->shaders.line.bind();
        this->debug.flush(this->cam.matrix(), this->shaders.line);

        hud.draw();
    }

    void GameScene::update(double dt) {
        Scene::update(dt);

#ifdef NDEBUG
        this->thirdperson = !this->ply || !this->ply->alive || this->ply->team == net::TEAM::SPECTATOR;
#else
        this->thirdperson |= !this->ply || !this->ply->alive || this->ply->team == net::TEAM::SPECTATOR;
#endif


        for (auto &kv : this->teams) {
            kv.second.update_players(*this);
        }

        this->world.update(dt);

        this->cam.update(dt, this->uniforms);
        if (this->ply) {
            this->ply->set_orientation(ace::draw2vox(this->cam.forward));
        }
        for (auto &kv : this->players) {
            kv.second->update(dt);
        }
        this->cam.update_view(this->uniforms);
        // TODO this is ugly
        if(this->ply) {
            this->ply->set_orientation(ace::draw2vox(this->cam.forward));
        }

        for (auto &kv : this->entities) {
            kv.second->update(dt);
        }

        this->hud.update(dt);
    }

    void GameScene::fixed_update(double dt) {
        Scene::fixed_update(dt);

        for (auto &kv : this->players) {
            kv.second->fixed_update(dt);
        }
    }

    void GameScene::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        this->hud.on_key(scancode, modifiers, pressed);

        if (pressed) {
#ifndef NDEBUG
            net::WEAPON wep = net::WEAPON::INVALID;
#endif
            if (this->ply) {
                switch (scancode) {
                case SDL_SCANCODE_1: this->ply->set_tool(net::TOOL::SPADE); break;
                case SDL_SCANCODE_2: this->ply->set_tool(net::TOOL::BLOCK); break;
                case SDL_SCANCODE_3: this->ply->set_tool(net::TOOL::WEAPON); break;
                case SDL_SCANCODE_4: this->ply->set_tool(net::TOOL::GRENADE); break;
                default: break;
                }

                if (scancode == this->client.config.get_key("reload", SDL_SCANCODE_R))
                    this->ply->get_tool()->reload();
            }

#ifndef NDEBUG
            switch(scancode) {
            case SDL_SCANCODE_5: wep = net::WEAPON::SEMI; break;
            case SDL_SCANCODE_6: wep = net::WEAPON::SMG; break;
            case SDL_SCANCODE_7: wep = net::WEAPON::SHOTGUN; break;
            case SDL_SCANCODE_F2: this->thirdperson = !this->thirdperson; break;
            case SDL_SCANCODE_F3: if(this->ply) this->ply->alive = !this->ply->alive; break;
            default: break;
            }

            if(wep != net::WEAPON::INVALID) {
                this->send_this_player(net::TEAM::TEAM1, wep);
            }
#endif
        }
    };

    void GameScene::on_mouse_motion(int x, int y, int dx, int dy) {
        this->hud.on_mouse_motion(x, y, dx, dy);
    }

    void GameScene::on_mouse_button(int button, bool pressed) {
        this->hud.on_mouse_button(button, pressed);
    }

    void GameScene::on_window_resize(int ow, int oh) {
        this->hud.on_window_resize(ow, oh);
        glViewport(0, 0, this->client.width(), this->client.height());
        this->set_zoom(false);
    }

    void GameScene::on_net_event(net::NetState event) {
        if(event == net::NetState::MAP_TRANSFER) {
            this->client.set_scene<LoadingScene>("aos://0:0");
        }
    }

    void GameScene::on_packet(net::PACKET type, std::unique_ptr<net::Loader> ploader) {
        // this is bad I KNOW dont flame thanks :))
        net::Loader *loader = ploader.get();

        switch(type) {
        case net::PACKET::CreatePlayer: {
            auto *pkt = static_cast<net::CreatePlayer *>(loader);
            auto *ply = this->get_ply(pkt->pid, true, pkt->pid == this->state_data.pid);
            if (ply->local_player) this->ply = ply; // this->ply() should be a function tbh
            ply->pid = pkt->pid;
            ply->set_team(pkt->team);
            ply->name = pkt->name;
            ply->set_weapon(pkt->weapon);
            ply->set_tool(net::TOOL::WEAPON);
            ply->set_position(pkt->position);
            ply->set_alive(ply->team != net::TEAM::SPECTATOR);
        } break;
        case net::PACKET::ExistingPlayer: {
            auto *pkt = static_cast<net::ExistingPlayer *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            ply->pid = pkt->pid;
            ply->set_team(pkt->team);
            ply->name = pkt->name;
            ply->set_weapon(pkt->weapon);
            ply->set_tool(pkt->tool);
            ply->set_color(pkt->color);
            ply->kills = pkt->kills;
        } break;
        case net::PACKET::WorldUpdate: {
            auto *pkt = static_cast<net::WorldUpdate *>(loader);
            for(int i = 0; i < 32; i++) {
                if (i == this->state_data.pid) continue;
                auto *p = this->get_ply(i, false);
                if (p == nullptr || !p->alive || p == this->ply) continue;

                const auto &wud = pkt->items.at(i);
                p->set_position(wud.first);
                p->set_orientation(wud.second);
            }
        } break;
        case net::PACKET::BlockAction: {
            auto *pkt = static_cast<net::BlockAction *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            if(pkt->value == net::ACTION::BUILD) {
                this->build_point(pkt->position.x, pkt->position.y, pkt->position.z, ply ? ply->color : this->block_colors[pkt->pid], true);
                ply->blocks.primary_ammo = std::max(0, ply->blocks.primary_ammo - 1);
            } else {
                this->destroy_point(pkt->position.x, pkt->position.y, pkt->position.z, pkt->value, true);
                if(pkt->value == net::ACTION::DESTROY) {
                    ply->blocks.primary_ammo = std::min(ply->blocks.max_primary(), ply->blocks.primary_ammo + 1);
                }
            }
        } break;
        case net::PACKET::BlockLine: {
            auto *pkt = static_cast<net::BlockLine *>(loader);
            auto *ply = this->get_ply(pkt->pid);
            std::vector<glm::ivec3> blocks = ace::block_line(pkt->start, pkt->end);
            ply->blocks.primary_ammo = std::max(0, ply->blocks.primary_ammo - int(blocks.size()));
            for(auto &block : blocks) {
                this->build_point(block.x, block.y, block.z, ply ? ply->color : this->block_colors[pkt->pid], true);
            }
        } break;
        case net::PACKET::InputData: {
            auto *pkt = static_cast<net::InputData *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->mf = pkt->up; ply->mb = pkt->down; ply->ml = pkt->left; ply->mr = pkt->right;
            ply->jump = pkt->jump; ply->sneak = pkt->sneak; ply->sprint = pkt->sprint;
            ply->set_crouch(pkt->crouch);
        } break;
        case net::PACKET::KillAction: {
            auto *pkt = static_cast<net::KillAction *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            auto *killer = this->get_ply(pkt->killer, false);
            if (ply == nullptr || killer == nullptr) break;
            ply->set_alive(false);
            fmt::print("{} killed {}. Respawning in {}\n", killer->name, ply->name, pkt->respawn_time);
            this->hud.add_killfeed_message(*killer, *ply, pkt->type);

            if(this->ply && ply->pid == this->ply->pid) {
                this->hud.respawn_time = pkt->respawn_time;
            }

            if(ply != killer) {
                killer->kills++;
            }
        } break;
        case net::PACKET::PositionData: {
            if (this->ply) {
                this->ply->set_position(static_cast<net::PositionData *>(loader)->position);
            }
        } break;
        case net::PACKET::WeaponInput: {
            auto *pkt = static_cast<net::WeaponInput *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->primary_fire = pkt->primary;
            ply->secondary_fire = pkt->secondary;
        } break;
        case net::PACKET::SetHP: {
            auto *pkt = static_cast<net::SetHP *>(loader);
            if (!this->ply) return;

            this->ply->health = pkt->hp;
            if (pkt->type != net::DAMAGE::FALL) {
                this->client.sound.play("hitplayer.wav", {}, 100, true);

                this->hud.set_hit(pkt->source);
            }
        }  break;
        case net::PACKET::GrenadePacket: {
            auto *pkt = static_cast<net::GrenadePacket *>(loader);
            this->create_object<world::Grenade>(pkt->position, pkt->velocity, pkt->fuse);
        } break;
        case net::PACKET::SetTool: {
            auto *pkt = static_cast<net::SetTool *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) break;
            ply->set_tool(pkt->tool);
        } break;
        case net::PACKET::SetColor: {
            auto *pkt = static_cast<net::SetColor *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply == nullptr) {
                this->block_colors[pkt->pid] = pkt->color;
            } else {
                ply->set_color(pkt->color);
            }
        } break;
        case net::PACKET::ChatMessage: {
            auto *pkt = static_cast<net::ChatMessage *>(loader);
            if(pkt->pid >= 32 || pkt->type == net::CHAT::SYSTEM) {
                this->hud.add_chat_message(fmt::format("[*]: {}", pkt->message), {1, 0, 0});
                return;
            }

            auto *ply = this->get_ply(pkt->pid, false);
            
            if (ply == nullptr) {
                fmt::print("chat missed from {}: {}\n", pkt->pid, pkt->message);
                break;
            }

            std::string msg;
            glm::vec3 color;
            if(pkt->type == net::CHAT::ALL) {
                msg = fmt::format("{} ({}): {}", ply->name, teams[ply->team].name, pkt->message);
                color = {1, 1, 1};
            } else {
                msg = fmt::format("{}: {}", ply->name, pkt->message);
                color = teams[ply->team].float_color;
            }
            this->hud.add_chat_message(msg, color);
            fmt::print("{}\n", msg);
        } break;
        case net::PACKET::MoveObject: {
            auto *pkt = static_cast<net::MoveObject *>(loader);
            auto *ent = this->get_ent(uint8_t(pkt->type));
            if (ent == nullptr) return; // ??? prob wrong gamemode
            ent->set_team(pkt->state);
            ent->set_position(pkt->position);
        } break;
        case net::PACKET::PlayerLeft: {
            auto *pkt = static_cast<net::PlayerLeft *>(loader);
            this->players.erase(pkt->pid);
        } break;
        case net::PACKET::TerritoryCapture: break;
        case net::PACKET::ProgressBar: break;
        case net::PACKET::IntelCapture: {
            auto *pkt = static_cast<net::IntelCapture *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);

            if (!pkt->winning) {
                if (ply != nullptr) {
                    auto msg = fmt::format("{} captured the {} team Intel!", ply->name, this->get_team(ply->team, true).name);
                    this->hud.add_chat_message(msg, { 1, 0, 0 });
                }
            } else {
                this->hud.set_big_message(fmt::format("{} Team Wins!", this->get_team(ply->team).name));
            }

            ply->kills += 10;
            this->get_team(ply->team).score++;

            this->client.sound.play_local(pkt->winning ? "horn.wav" : "pickup.wav");
        } break;
        case net::PACKET::IntelPickup: {
            auto *pkt = static_cast<net::IntelPickup *>(loader);

            auto *ply = this->get_ply(pkt->pid, false);
            if (ply != nullptr) {
                auto msg = fmt::format("{} has the {} Intel", ply->name, this->get_team(ply->team, true).name);
                this->hud.add_chat_message(msg, { 1, 0, 0 });
            }
            this->client.sound.play_local("pickup.wav");
            auto *ent = this->get_ent(uint8_t(ply->team == net::TEAM::TEAM1 ? net::OBJECT::GREEN_FLAG : net::OBJECT::BLUE_FLAG));
            if(ent != nullptr)
                ent->set_carrier(ply->pid);
        } break;
        case net::PACKET::IntelDrop: {
            auto *pkt = static_cast<net::IntelDrop *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (ply != nullptr) {
                auto msg = fmt::format("{} has dropped the {} Intel", ply->name, this->get_team(ply->team, true).name);
                this->hud.add_chat_message(msg, { 1, 0, 0 });
            }
            auto *ent = this->get_ent(uint8_t(ply->team == net::TEAM::TEAM1 ? net::OBJECT::GREEN_FLAG : net::OBJECT::BLUE_FLAG));
            if (ent == nullptr) return;
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
            auto *pkt = static_cast<net::WeaponReload *>(loader);
            auto *ply = this->get_ply(pkt->pid, false);
            if (!ply || !ply->weapon_obj) break;
            ply->weapon_obj->on_reload(pkt);
        } break;
        default: ;
        }
    }

    bool GameScene::on_text_typing(const std::string& text) {
        return this->hud.on_text_typing(text);
    }

    void GameScene::on_text_finished(bool cancelled) {
        this->hud.on_text_finished(cancelled);
    }

    bool GameScene::build_point(int x, int y, int z, glm::u8vec3 color, bool s2c) {
        if (s2c) {
            this->client.sound.play("build.wav", vox2draw(glm::vec3{ x, y, z } + 0.5f), 50);
            return this->world.build_block(x, y, z, color, s2c);
        }
        this->send_block_action(x, y, z, net::ACTION::BUILD);
        return false;
    }

    bool GameScene::destroy_point(int x, int y, int z, net::ACTION type, bool s2c) {
        if(!s2c) {
            this->send_block_action(x, y, z, type);
            return false;
        }

        return this->world.destroy_block(x, y, z, type);
    }

    bool GameScene::damage_point(int x, int y, int z, uint8_t damage, Face f, bool allow_destroy) {
        if(f != Face::INVALID) {
            this->create_object<world::DebrisGroup>(draw::get_face(x, y, z, f), glm::vec3(unpack_argb(this->world.get_color(x, y, z))), 0.25f, 4);
        }

        if(damage && this->world.damage_block(x, y, z, damage, false) && allow_destroy) {
            this->send_block_action(x, y, z, net::ACTION::DESTROY);
            return true;
        }
        return false;
    }

    void GameScene::set_zoom(bool zoom) {
        this->cam.sensitivity = zoom ? this->cam.zoom_sensitivity : this->cam.normal_sensitivity;
        this->cam.set_projection(zoom ? 37.5f : 75.0f, this->client.width(), this->client.height(), 0.1f, 128.f);
    }

    void GameScene::send_block_action(int x, int y, int z, net::ACTION type) const {
        if (type == net::ACTION::GRENADE) return;

        net::BlockAction ba;
        ba.position = { x, y, z };
        ba.value = type;
        this->client.net.send_packet(ba);
    }

    void GameScene::send_block_line(glm::ivec3 p1, glm::ivec3 p2) const {
        net::BlockLine bl;
        bl.start = p1;
        bl.end = p2;
        this->client.net.send_packet(bl);
    }

    void GameScene::send_position_update() const {
        if (!this->ply || !this->ply->alive) return;

        net::PositionData pd;
        pd.position = this->ply->p;
        this->client.net.send_packet(pd, ENET_PACKET_FLAG_UNSEQUENCED);
    }

    void GameScene::send_orientation_update() const {
        if (!this->ply || !this->ply->alive) return;

        net::OrientationData od;
        od.orientation = this->ply->f;
        this->client.net.send_packet(od, ENET_PACKET_FLAG_UNSEQUENCED);
    }

    void GameScene::send_input_update() const {
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
        this->client.net.send_packet(id);

        net::WeaponInput wi;
        wi.primary = this->ply->primary_fire;
        wi.secondary = this->ply->secondary_fire;
        this->client.net.send_packet(wi);
    }

    void GameScene::send_grenade(float fuse) const {
        if (!this->ply || !this->ply->alive) return;

        net::GrenadePacket gp;
        gp.position = this->ply->p;
        gp.velocity = this->ply->f + this->ply->v;
        gp.fuse = fuse;
        this->client.net.send_packet(gp);
    }

    void GameScene::send_team_change(net::TEAM new_team) const {
        net::ChangeTeam ct;
        ct.team = new_team;
        this->client.net.send_packet(ct);
    }

    void GameScene::send_weapon_change(net::WEAPON new_weapon) const {
        net::ChangeWeapon cw;
        cw.weapon = new_weapon;
        this->client.net.send_packet(cw);
    }

    void GameScene::send_this_player(net::TEAM team, net::WEAPON weapon) const {
        net::ExistingPlayer ep;
        ep.name = this->ply_name;
        ep.team = team;
        ep.weapon = weapon;
        this->client.net.send_packet(ep);
    }

    RaycastResult GameScene::cast_ray(glm::vec3 origin, glm::vec3 dir, world::DrawPlayer *exclude) {
        RaycastResult r;

        for (auto &kv : this->players) {
            if (!kv.second->alive || kv.second.get() == exclude) continue;
            if (glm::dot(dir, kv.second->e - origin) < 0) continue;

            
            r.type = kv.second->test_ray(origin, dir, &r.hit);

            if (r.type != net::HIT::INVALID) {
                r.ply = kv.second.get();
                return r;
            }
        }

        return { nullptr, net::HIT::INVALID, glm::vec3(-1) };
    }

    void GameScene::respawn_entities() {
        this->entities.clear();
        if (this->state_data.mode == 0) {
            auto &mode = this->state_data.state.ctf;
            // uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier
            this->entities.emplace(uint8_t(net::OBJECT::BLUE_BASE), std::make_unique<world::CommandPost>(*this, 0, mode.team1_base, net::TEAM::TEAM1, 255));
            this->entities.emplace(uint8_t(net::OBJECT::GREEN_BASE), std::make_unique<world::CommandPost>(*this, 0, mode.team2_base, net::TEAM::TEAM2, 255));
            this->entities.emplace(uint8_t(net::OBJECT::BLUE_FLAG), std::make_unique<world::Flag>(*this, 0, mode.team1_flag, net::TEAM::TEAM1, mode.team1_carrier));
            this->entities.emplace(uint8_t(net::OBJECT::GREEN_FLAG), std::make_unique<world::Flag>(*this, 0, mode.team2_flag, net::TEAM::TEAM2, mode.team2_carrier));

            auto &t1 = this->get_team(net::TEAM::TEAM1);
            t1.score = mode.team1_score; t1.max_score = mode.cap_limit;
            auto &t2 = this->get_team(net::TEAM::TEAM2);
            t2.score = mode.team2_score; t2.max_score = mode.cap_limit;
        }
    }

    void GameScene::set_fog_color(glm::vec3 color) {
        glClearColor(color.r, color.g, color.b, 1.0f);
        this->uniforms->light_pos = normalize(glm::vec3{ -0.16, 0.8, 0.56 });
        this->uniforms->fog_color = color;
    }
}}
