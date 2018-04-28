#include "glm/glm.hpp"

#include "game_client.h"
#include "scene/hud.h"
#include "scene/game.h"

namespace ace { namespace scene {
    namespace {
         void write_killer_weapon(const world::DrawPlayer &killer, net::KILL kill_type, fmt::MemoryWriter &writer) {
            switch (kill_type) {
            case net::KILL::HEADSHOT:
                writer.write("(Headshot)");
                break;
            case net::KILL::GRENADE:
                writer.write("(Grenade)");
                break;
            case net::KILL::MELEE:
                writer.write("(Spade)");
                break;
            case net::KILL::WEAPON:
                writer.write("({})", killer.weapon_obj->name());
                break;
            default:
                break;
            }
        }

        glm::u8vec3 color_palette[] = {
            { 15, 15, 15 },{ 47, 47, 47 },{ 79, 79, 79 },{ 111, 111, 111 },{ 143, 143, 143 },{ 175, 175, 175 },{ 207, 207, 207 },{ 239, 239, 239 },
            { 31, 0, 0 },{ 95, 0, 0 },{ 159, 0, 0 },{ 223, 0, 0 },{ 255, 31, 31 },{ 255, 95, 95 },{ 255, 159, 159 },{ 255, 223, 223 },
            { 31, 15, 0 },{ 95, 47, 0 },{ 159, 79, 0 },{ 223, 111, 0 },{ 255, 143, 31 },{ 255, 175, 95 },{ 255, 207, 159 },{ 255, 239, 223 },
            { 31, 31, 0 },{ 95, 95, 0 },{ 159, 159, 0 },{ 223, 223, 0 },{ 255, 255, 31 },{ 255, 255, 95 },{ 255, 255, 159 },{ 255, 255, 223 },
            { 0, 31, 0 },{ 0, 95, 0 },{ 0, 159, 0 },{ 0, 223, 0 },{ 31, 255, 31 },{ 95, 255, 95 },{ 159, 255, 159 },{ 223, 255, 223 },
            { 0, 31, 31 },{ 0, 95, 95 },{ 0, 159, 159 },{ 0, 223, 223 },{ 31, 255, 255 },{ 95, 255, 255 },{ 159, 255, 255 },{ 223, 255, 255 },
            { 0, 0, 31 },{ 0, 0, 95 },{ 0, 0, 159 },{ 0, 0, 223 },{ 31, 31, 255 },{ 95, 95, 255 },{ 159, 159, 255 },{ 223, 223, 255 },
            { 31, 0, 31 },{ 95, 0, 95 },{ 159, 0, 159 },{ 223, 0, 223 },{ 255, 31, 255 },{ 255, 95, 255 },{ 255, 159, 255 },{ 255, 223, 255 }
        };

        draw::SpriteGroup *gen_palette(draw::SpriteManager &manager) {
            const auto colors = sizeof(color_palette) / sizeof(color_palette[0]);
            glm::u8vec4 pixels[colors * colors];
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    auto color = color_palette[i * 8 + j];
                    for (int x = i * 8 + 1; x < i * 8 + 7; x++) {
                        for (int y = j * 8 + 1; y < j * 8 + 7; y++) {
                            pixels[x * colors + y] = glm::u8vec4(color, 255);
                        }
                    }
                }
            }
            auto spr = manager.get("palette", SDL_CreateRGBSurfaceFrom(pixels, colors, colors, 32, 4 * colors, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24));
            spr->set_antialias(false);
            return spr;
        }

        draw::SpriteGroup *gen_palret(draw::SpriteManager &manager) {
            const auto siz = 8u;
            glm::u8vec4 pixels[siz * siz]{};
            for (int i = 0; i < 7; i++) {
                pixels[0 * siz + i] = { 255, 255, 255, 255 };
                pixels[(7 - i) * siz + 0] = { 255, 255, 255, 255 };
                pixels[i * siz + 7] = { 255, 255, 255, 255 };
                pixels[7 * siz + (7 - i)] = { 255, 255, 255, 255 };
            }
            auto spr = manager.get("paletteret", SDL_CreateRGBSurfaceFrom(pixels, siz, siz, 32, 4 * siz, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24));
            spr->set_antialias(false);
            return spr;
        }

        float angle2d(glm::vec2 p, glm::vec2 o) {
            return glm::degrees(glm::atan(glm::determinant(glm::mat2(o, p)), dot(o, p)));
        }
    }

    MapDisplay::MapDisplay(HUD& hud): hud(hud), marker(hud.sprites.get("player.bmp")), map(hud.scene.map.get_overview()), big(map), mini(map) {
        this->map->order = draw::Layer::FOREGROUND + 4;
        this->marker->order = this->map->order + 1;
        
        this->big.alignment = draw::Align::CENTER;
        this->mini.alignment = draw::Align::TOP_RIGHT;
        this->mini.scale = glm::vec2{ 0.25f };
    }

    void MapDisplay::update(double dt) {
        this->big.position.x = hud.scene.client.width() / 2.f;
        this->big.position.y = hud.scene.client.height() / 2.f;
        this->mini.position = { hud.scene.client.width() - 15, 15 };
        glm::vec2 d(draw2vox(this->hud.scene.cam.position));
        this->mini.set_region(d - glm::vec2{64}, d + glm::vec2{64});
    }

    void MapDisplay::draw() {
        glm::vec2 offset;
        if(this->big_open) {
            offset = this->big.get_position();
            this->big.draw();
            this->draw_map_grid(offset);
        } else {
            offset = this->mini.get_position(draw::Align::CENTER);
            this->mini.draw();
        }

        glm::vec2 player_pos(draw2vox(this->hud.scene.cam.position));

        for (auto &kv : this->hud.scene.players) {
            auto &ply = kv.second;
#ifdef NDEBUG
            if (this->hud.scene.ply && ply->team != this->hud.scene.ply->team) continue;
#endif
            if (!ply->alive) continue;

            glm::vec2 p(ply->p);
            if(!this->big_open) {
                p -= player_pos;
                if (p.x < -64 || p.x > 64 || p.y < -64 || p.y > 64) continue;
            }

            auto color = ply->local_player ? glm::vec4(0, 1.0f, 1.0f, 1.0f) : glm::vec4(this->hud.scene.get_team(ply->team).float_color, 1.0f);
            this->marker->draw(color, offset + p, dir2ang(ply->draw_forward).x + 90.f, {1, 1}, draw::Align::CENTER);
        }

        for(auto &kv : this->hud.scene.entities) {
            auto &ent = kv.second;
            if (!ent->visible()) continue;

            glm::vec2 p(ent->position);
            if(!this->big_open) {
                p = clamp(p - player_pos, -64.f, 64.f);
            }

            auto *spr = this->hud.sprites.get(ent->icon());
            spr->order = this->marker->order + 1;
            spr->draw(glm::vec4(this->hud.scene.get_team(ent->team).float_color, 1.0f), offset + p, 0, { 1, 1 }, draw::Align::CENTER);
        }
    }

    void MapDisplay::draw_map_grid(glm::vec2 offset) {
        for (char c = 'A'; c <= 'H'; c++) {
            float x = (offset.x + (32 + 64 * (c - 'A'))) * this->big.scale.x;
            this->hud.sys15->draw(std::string(1, c), { x, offset.y }, { 1, 1, 1 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        }

        for (int c = 1; c <= 8; c++) {
            float y = offset.y + (32 + 64 * (c - 1));
            this->hud.sys15->draw(std::to_string(c), { offset.x, y }, { 1, 1, 1 }, { 1,1 }, draw::Align::BOTTOM_RIGHT);
        }
    }

    HUD::HUD(GameScene& s) :
        scene(s),
        sprites(scene.client.sprites), reticle(sprites.get("target.png")), pal(gen_palette(sprites)), palret(gen_palret(sprites)),
        hit_indicator(sprites.get("indicator.bmp")), weapon_sight(sprites.get("semi.png")),
        ammo_icon(sprites.get("semi.bmp")),
        map_display(*this),
        ply(scene, false),
        sys48(scene.client.fonts.get("fixedsys.ttf", 48, false)),
        sys13(scene.client.fonts.get("fixedsys.ttf", 13, false)),
        sys15(scene.client.fonts.get("fixedsys.ttf", 15, false)),
        sys18(scene.client.fonts.get("fixedsys.ttf", 16, false)) {
       
        this->reticle.alignment = draw::Align::CENTER;
        this->hit_indicator.alignment = draw::Align::CENTER;
        this->weapon_sight.alignment = draw::Align::CENTER;
        this->ammo_icon.alignment = draw::Align::BOTTOM_RIGHT;
        this->pal.alignment = draw::Align::BOTTOM_RIGHT;
        this->ammo_icon.tint = { 1, 1, 0, 1 };
        
        this->on_window_resize(0, 0);
    }

    void HUD::update(double dt) {
        if (this->scene.client.keyboard.keys[SDL_SCANCODE_F6]) return;

        this->map_display.update(dt);

        this->big_message_time = std::max(0.0, this->big_message_time - dt);

        if (!this->scene.ply) return;

        if (!this->scene.ply->alive) {
            float old_rt = this->respawn_time;
            this->respawn_time = std::max(1.0, this->respawn_time - dt);
            int rt = int(this->respawn_time);
            if (int(old_rt) != rt) {
                if (rt == 3 || rt == 2) {
                    this->scene.client.sound.play("beep2.wav", {}, 100, true);
                } else if (rt == 1) {
                    this->scene.client.sound.play("beep1.wav", {}, 100, true);
                }
            }
        }
        this->hit_indicator.rotation = angle2d(this->last_hit - this->scene.ply->p, this->scene.ply->f);
        this->hit_indicator.tint.a = std::max(0.0, this->hit_indicator.tint.a - dt);
    }

    void HUD::draw() {
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (this->scene.client.keyboard.keys[SDL_SCANCODE_F6]) return;

        if (this->scene.ply && !this->scene.ply->alive) {
            this->sys48->draw_shadowed(fmt::format("INSERT COIN:{}", int(this->respawn_time)), { scene.client.width() / 2.f, scene.client.height() - 20 }, { 1, 0, 0 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        } else if(this->scene.ply) {
            this->sys48->draw_shadowed(std::to_string(this->scene.ply->health), { scene.client.width() / 2.f, scene.client.height() - 20 }, scene.ply->health <= 20 ? glm::vec3{ 1, 0, 0 } : glm::vec3{ 1, 1, 1 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);

            this->ammo_icon.position = { scene.client.width(), scene.client.height() - 20 };
            if (this->scene.ply->tool == net::TOOL::BLOCK) {
                this->pal.draw();
                this->palret.tint = glm::vec4{ float(int(scene.time * 2) % 2) };
                this->palret.tint.a = 1.0f;
                this->palret.draw();

                this->ammo_icon.position.x -= pal.w();
            }

            auto pos = glm::vec2(ammo_icon.position.x - ammo_icon.w(), ammo_icon.position.y);
            this->sys48->draw_shadowed(this->scene.ply->get_tool()->display_ammo(), pos, { 1, 1, 0 }, { 1, 1 }, draw::Align::BOTTOM_RIGHT);

            


            this->hit_indicator.draw();

            this->ammo_icon.draw();

            if(this->scene.ply->secondary_fire && this->scene.ply->weapon_equipped)
                this->weapon_sight.draw();
            else
                this->reticle.draw();
        }


        std::string str;
        switch(state) {
        case State::Exit:
            str = "EXIT GAME? Y/N";
            break;
        case State::ChangeTeam:
            str = fmt::format("TEAM: 1 - {}/2 - {}/3 - Spectator", this->scene.teams[net::TEAM::TEAM1].name, this->scene.teams[net::TEAM::TEAM2].name);
            break;
        case State::ChangeWeapon:
            str = "WEAPON: 1 - Rifle/2 - SMG/3 - Shotgun";
            break;
        default:
            break;
        }
        if(!str.empty()) {
            this->sys48->draw(str, { this->scene.client.width() / 2.f, this->scene.client.height() / 2.5f }, { 1, 0, 0 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        }

        if(big_message_time > 0.f) {
            this->sys48->draw(big_message, { this->scene.client.width() / 2.f, this->scene.client.height() / 1.75f }, { 1, 0, 0 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        }

        this->draw_chat();

        if (this->scene.client.keyboard.keys[this->scene.client.config.get_key("scoreboard")]) {
            this->draw_scoreboard();
        }
        
        
        this->map_display.draw();

        this->scene.shaders.sprite.bind();
        this->scene.shaders.sprite.uniform("projection", projection);
        this->sprites.draw(scene.shaders.sprite);
        
        this->scene.shaders.text.bind();
        this->scene.client.fonts.draw(projection, scene.shaders.text);
    }

    void HUD::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        if (!pressed) return;
        this->update_color(scancode);

        if(scancode == this->scene.client.config.get_key("all_chat")) {
            this->cur_chat_type = net::CHAT::ALL;
            this->scene.client.tasks.call_later(0, [this] { this->scene.client.toggle_text_input(); });
        } else if(scancode == this->scene.client.config.get_key("team_chat")) {
            this->cur_chat_type = net::CHAT::TEAM;
            this->scene.client.tasks.call_later(0, [this] { this->scene.client.toggle_text_input(); });
        } else if(scancode == this->scene.client.config.get_key("map")) {
            this->map_display.big_open = !this->map_display.big_open;
        } else if (scancode == this->scene.client.config.get_key("change_team")) {
            this->state = State::ChangeTeam;
        } else if (scancode == this->scene.client.config.get_key("change_weapon")) {
            this->state = State::ChangeWeapon;
        }

        if(this->state == State::Exit) {
            if(scancode == SDL_SCANCODE_Y)
                this->scene.client.quit();
            else if(scancode == SDL_SCANCODE_N)
                this->state = State::None;
        }

        if(scancode == SDL_SCANCODE_ESCAPE)
            this->state = this->state == State::None ? State::Exit : State::None;

        if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_3) {
            if(this->state == State::ChangeWeapon) {
                this->scene.send_weapon_change(net::WEAPON(scancode - SDL_SCANCODE_1));
            } else if(this->state == State::ChangeTeam) {
                auto team = scancode == SDL_SCANCODE_3 ? net::TEAM::SPECTATOR : net::TEAM(scancode - SDL_SCANCODE_1);
                this->scene.send_team_change(team);
            } else {
                return;
            }
            this->state = State::None;
        }
    }

    void HUD::on_mouse_button(int button, bool pressed) {
    }

    void HUD::on_mouse_motion(int x, int y, int dx, int dy) {
    }

    bool HUD::on_text_typing(const std::string& text) {
        return this->scene.client.input_buffer.length() < 90;
    }

    void HUD::on_text_finished() {
        if (this->cur_chat_type == net::CHAT::INVALID || this->scene.client.input_buffer.empty()) return;

        net::ChatMessage cm;
        cm.type = this->cur_chat_type;
        cm.message = this->scene.client.input_buffer;
        this->scene.client.net.send_packet(net::PACKET::ChatMessage, cm);

        this->cur_chat_type = net::CHAT::INVALID;
    }

    void HUD::on_window_resize(int ow, int oh) {
        glm::vec2 size = this->scene.client.size();
        this->projection = glm::ortho(0.f, size.x, size.y, 0.0f);

        this->weapon_sight.position = this->hit_indicator.position = this->reticle.position = size / 2.f;
        this->pal.position = size;

        this->weapon_sight.scale = { size.y / this->weapon_sight.group->h, size.y / this->weapon_sight.group->h };
        this->hit_indicator.scale = { size.y / this->hit_indicator.group->h, size.y / this->hit_indicator.group->h };
    }

    void HUD::add_chat_message(std::string message, glm::vec3 color) {
        this->chat_messages.push_front({std::move(message), color});
        while (this->chat_messages.size() > 6) {
            this->chat_messages.pop_back();
        }
    }

    void HUD::add_killfeed_message(const world::DrawPlayer &killer, const world::DrawPlayer &victim, net::KILL kill_type) {
        fmt::MemoryWriter message;
        switch (kill_type) {
        case net::KILL::FALL:
            message.write("{} fell too far", victim.name);
            break;
        case net::KILL::TEAM_CHANGE:
            message.write("{} changed teams", victim.name);
            break;
        case net::KILL::CLASS_CHANGE:
            message.write("{} changed weapons", victim.name);
            break;
        default:
            message.write("{} killed {} ", killer.name, victim.name);
            write_killer_weapon(killer, kill_type, message);
        }
        glm::vec3 color(0.f);
        if(victim.local_player || killer.local_player) {
            color.r = 1;
        } else {
            color = this->scene.get_team(killer.team).float_color;
        }

        this->add_chat_message(message.str(),  color);
    }

    void HUD::set_big_message(std::string message) {
        this->big_message = std::move(message);
        this->big_message_time = 4.0f;
    }

    void HUD::set_hit(glm::vec3 source) {
        this->last_hit = source;
        this->hit_indicator.tint.a = 1.2f;
    }

    void HUD::update_weapon(const std::string &sight) {
        this->weapon_sight.group = this->sprites.get(sight);
    }

    void HUD::update_tool(const std::string &ammo_icon) {
        this->ammo_icon.group = this->sprites.get(ammo_icon);
    }

    void HUD::update_color(SDL_Scancode key) {
        if (!this->scene.ply) return;

        switch(key) {
            case SDL_SCANCODE_UP:
                this->color_index -= 8;
                break;
            case SDL_SCANCODE_DOWN:
                this->color_index += 8;
                break;
            case SDL_SCANCODE_LEFT:
                this->color_index -= 1;
                break;
            case SDL_SCANCODE_RIGHT:
                this->color_index += 1;
                break;
            default:
                return;
        }

        this->color_index = (this->color_index + 64) % 64;
        this->scene.ply->set_color(color_palette[this->color_index]);

        glm::vec2 pos(this->color_index % 8, this->color_index / 8);
        this->palret.position = this->pal.get_position() + pos * 8.f;
    }

    void HUD::draw_chat() {
        for (auto i = chat_messages.begin(); i != chat_messages.end(); ++i) {
            int index = i - chat_messages.begin() + 1;
            glm::vec2 pos;
            pos.x = 25;
            pos.y = (scene.client.height() - 15 * index) - 20;
            this->sys13->draw(i->message, pos, i->color);
        }

        if (this->scene.client.text_input_active()) {
            const char *prefix = this->cur_chat_type == net::CHAT::ALL ? "(Global) " : "(Team): ";
            this->sys13->draw(prefix + this->scene.client.input_buffer + "_", { 25, scene.client.height() - 20 });
        }
    }

    inline void draw_scoreboard_players(Team &team, glm::vec2 offset, draw::Align alignment, draw::Font *f) {
        for (auto i = team.players.begin(); i != team.players.end(); ++i) {
            auto *ply = *i;
            f->draw(fmt::format("{:>15} #{:<2d} {:<3d}", ply->name, ply->pid, ply->kills), offset, { 1, 1, 1 }, { 1, 1 }, alignment);
            offset.y += f->size();
        }
    }

    void HUD::draw_scoreboard() {
        auto &t1 = this->scene.get_team(net::TEAM::TEAM1);
        auto &t2 = this->scene.get_team(net::TEAM::TEAM2);
        float w = this->scene.client.width();
        this->sys48->draw(fmt::format("{}-{}", t1.score, t1.max_score), { w / 4, 150 }, t1.float_color, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        this->sys48->draw(fmt::format("{}-{}", t2.score, t2.max_score), { w - w / 4, 150 }, t2.float_color, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        draw_scoreboard_players(t1, { w * .1, 175 }, draw::Align::BOTTOM_LEFT, this->sys18);
        draw_scoreboard_players(t2, { w - w * .1, 175 }, draw::Align::BOTTOM_RIGHT, this->sys18);
    }

}}