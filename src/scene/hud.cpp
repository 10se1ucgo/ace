#include "scene/hud.h"
#include "scene/game.h"

#include "glm/glm.hpp"

namespace ace { namespace scene {
    namespace {
         void write_killer_weapon(world::DrawPlayer &killer, net::KILL kill_type, fmt::MemoryWriter &writer) {
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
    }

    glm::u8vec3 color_palette[] = {
        {15, 15, 15}, {47, 47, 47}, {79, 79, 79}, {111, 111, 111}, {143, 143, 143}, {175, 175, 175}, {207, 207, 207}, {239, 239, 239},
        {31, 0, 0}, {95, 0, 0}, {159, 0, 0}, {223, 0, 0}, {255, 31, 31}, {255, 95, 95}, {255, 159, 159}, {255, 223, 223},
        {31, 15, 0}, {95, 47, 0}, {159, 79, 0}, {223, 111, 0}, {255, 143, 31}, {255, 175, 95}, {255, 207, 159}, {255, 239, 223},
        {31, 31, 0}, {95, 95, 0}, {159, 159, 0}, {223, 223, 0}, {255, 255, 31}, {255, 255, 95}, {255, 255, 159}, {255, 255, 223},
        {0, 31, 0}, {0, 95, 0}, {0, 159, 0}, {0, 223, 0}, {31, 255, 31}, {95, 255, 95}, {159, 255, 159}, {223, 255, 223},
        {0, 31, 31}, {0, 95, 95}, {0, 159, 159}, {0, 223, 223}, {31, 255, 255}, {95, 255, 255}, {159, 255, 255}, {223, 255, 255},
        {0, 0, 31}, {0, 0, 95}, {0, 0, 159}, {0, 0, 223}, {31, 31, 255}, {95, 95, 255}, {159, 159, 255}, {223, 223, 255},
        {31, 0, 31}, {95, 0, 95}, {159, 0, 159}, {223, 0, 223}, {255, 31, 255}, {255, 95, 255}, {255, 159, 255}, {255, 223, 255}
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

        return manager.get("palette", SDL_CreateRGBSurfaceFrom(pixels, colors, colors, 32, 4 * colors, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24));
    }

    draw::SpriteGroup *gen_palret(draw::SpriteManager &manager) {
        glm::u8vec4 pixels[8 * 8]{};
        for(int i = 0; i < 7; i++) {
            pixels[0 * 8 + i] = { 255, 255, 255, 255 };
            pixels[(7 - i) * 8 + 0] = { 255, 255, 255, 255 };
            pixels[i * 8 + 7] = { 255, 255, 255, 255 };
            pixels[7 * 8 + (7 - i)] = { 255, 255, 255, 255 };
        }
        return manager.get("paletteret", SDL_CreateRGBSurfaceFrom(pixels, 8, 8, 32, 4 * 8, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24));
    }

    MapDisplay::MapDisplay(HUD& hud): hud(hud), big(&hud.scene.map.overview), marker(hud.sprites.get("player.bmp")) {
        big.alignment = draw::Align::CENTER;
    }

    void MapDisplay::update(double dt) {
        big.position.x = hud.scene.client.width() / 2.f;
        big.position.y = hud.scene.client.height() / 2.f;
    }

    void MapDisplay::draw() {
        if (!visible) return;

        auto &shader = this->hud.scene.shaders.sprite;
        shader.bind();
        big.draw();
        big.flush(shader);
        auto op = big.get_position();
        for (auto &kv : this->hud.scene.players) {
            auto &ply = kv.second;
            if (!ply->alive) continue;
            auto color = ply->local_player ? glm::vec4(0, 1.0f, 1.0f, 1.0f) : glm::vec4(this->hud.scene.get_team(ply->team).float_color, 1.0f);
            marker->draw(color, op + glm::vec2{ ply->p.x, ply->p.y }, dir2ang(ply->draw_forward).x + 90.f, {1, 1}, draw::Align::CENTER);
        }
        marker->draw(shader);

        for(char c = 'A'; c <= 'H'; c++) {
            float x = (op.x + (32 + 64 * (c - 'A'))) * big.scale.x;
            float y = op.y;
            this->hud.sys8->draw(std::string(1, c), glm::vec2{ x, y }, {1, 1, 1}, { 1, 1}, draw::Align::BOTTOM_CENTER);
        }

        for (int c = 1; c <= 8; c++) {
            float x = op.x;
            float y = op.y + (32 + 64 * (c - 1));
            this->hud.sys8->draw(std::to_string(c), glm::vec2{ x, y }, { 1, 1, 1 }, { 1,1 }, draw::Align::BOTTOM_RIGHT);
        }
    }

    HUD::HUD(GameScene& s) :
        scene(s),
        reticle(sprites.get("target.png")), pal(gen_palette(sprites)), palret(gen_palret(sprites)), hit_indicator(sprites.get("indicator.bmp")),
        weapon_sight(sprites.get("semi.png")), ammo_icon(sprites.get("semi.bmp")),
        map_display(*this),
        sys48(scene.client.fonts.get("fixedsys.ttf", 48)),
        sys8(scene.client.fonts.get("fixedsys.ttf", 15)),
        sys15(scene.client.fonts.get("fixedsys.ttf", 15)) {
       
        reticle.alignment = draw::Align::CENTER;
        hit_indicator.alignment = draw::Align::CENTER;
        weapon_sight.alignment = draw::Align::CENTER;
        ammo_icon.alignment = draw::Align::BOTTOM_RIGHT;
        pal.alignment = draw::Align::BOTTOM_RIGHT;
        ammo_icon.tint = { 1, 1, 0, 1 };
        
        this->on_window_resize(0, 0);
    }

    void HUD::update(double dt) {
        if (this->scene.client.keyboard.keys[SDL_SCANCODE_F6]) return;



        if (!this->scene.ply->alive) {
            float old_rt = respawn_time;
            respawn_time = std::max(1.0, respawn_time - dt);
            int rt = int(respawn_time);
            if (int(old_rt) != rt) {
                if (rt == 3 || rt == 2) {
                    this->scene.client.sound.play("beep2.wav", {}, 100, true);
                }
                else if (rt == 1) {
                    this->scene.client.sound.play("beep1.wav", {}, 100, true);
                }
            }
        }

        glm::vec2 p(this->last_hit - this->scene.ply->p);
        glm::vec2 o(this->scene.ply->f);
        float angle = glm::degrees(glm::atan(glm::determinant(glm::mat2(o, p)), dot(o, p)));
        this->hit_indicator.rotation = angle;
        this->hit_indicator.tint.a = std::max(0.0f, this->hit_indicator.tint.a - float(dt));

        

        map_display.update(dt);
    }

    void HUD::draw() {
        if (this->scene.client.keyboard.keys[SDL_SCANCODE_F6]) return;

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        


        if (!this->scene.ply->alive) {
            this->sys48->draw(fmt::format("INSERT COIN:{}", int(this->respawn_time)), { scene.client.width() / 2.f, scene.client.height() - 20 }, { 1, 0, 0 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);
        }
        else {
            this->sys48->draw(std::to_string(this->scene.ply->health), { scene.client.width() / 2.f, scene.client.height() - 20 }, scene.ply->health <= 20 ? glm::vec3{ 1, 0, 0 } : glm::vec3{ 1, 1, 1 }, { 1, 1 }, draw::Align::BOTTOM_CENTER);

            this->ammo_icon.position = { scene.client.width(), scene.client.height() - 20 };
            if (this->scene.ply->tool == net::TOOL::BLOCK) {
                pal.draw();
                palret.tint = glm::vec4{ float(int(scene.time * 2) % 2) };
                palret.tint.a = 1.0f;
                palret.draw();

                ammo_icon.position.x -= pal.w();
            }

            auto pos = glm::vec2(ammo_icon.position.x - ammo_icon.w(), ammo_icon.position.y);
            this->sys48->draw(this->scene.ply->get_tool()->display_ammo(), pos, { 1, 1, 0 }, { 1, 1 }, draw::Align::BOTTOM_RIGHT);

            


            hit_indicator.draw();

            ammo_icon.draw();

            if(this->scene.ply->secondary_fire && this->scene.ply->weapon_equipped)
                weapon_sight.draw();
            else
                reticle.draw();
                
        }
        this->draw_chat();

        scene.shaders.sprite.bind();
        scene.shaders.sprite.uniform("projection", projection);
        this->sprites.draw(scene.shaders.sprite);

        map_display.draw();
        
        scene.shaders.text.bind();
        this->scene.client.fonts.draw(projection, scene.shaders.text);
    }

    void HUD::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        if (!pressed) return;

        switch(scancode) {
        case SDL_SCANCODE_UP:
            this->color_index -= 8;
            this->update_color();
            break;
        case SDL_SCANCODE_DOWN:
            this->color_index += 8;
            this->update_color();
            break;
        case SDL_SCANCODE_LEFT:
            this->color_index -= 1;
            this->update_color();
            break;
        case SDL_SCANCODE_RIGHT:
            this->color_index += 1;
            this->update_color();
            break;
        case SDL_SCANCODE_T:
            this->cur_chat_type = net::CHAT::ALL;
            this->scene.client.tasks.call_later(0, [this] { this->scene.client.toggle_text_input(); });
            break;
        case SDL_SCANCODE_Y:
            this->cur_chat_type = net::CHAT::TEAM;
            this->scene.client.tasks.call_later(0, [this] { this->scene.client.toggle_text_input(); });
            break;
        case SDL_SCANCODE_M:
            map_display.visible = !map_display.visible;
            break;
        default:
            break;
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
        this->scene.client.net->send_packet(net::PACKET::ChatMessage, cm);

        this->cur_chat_type = net::CHAT::INVALID;
    }

    void HUD::on_window_resize(int ow, int oh) {
        float w = scene.client.width(), h = scene.client.height();
        projection = glm::ortho(0.f, w, h, 0.0f);

        weapon_sight.position = hit_indicator.position = reticle.position = { w / 2.f, h / 2.f };
        pal.position = { w, h };
        weapon_sight.scale = { h / weapon_sight.group->h, h / weapon_sight.group->h };
    }

    void HUD::add_chat_message(const std::string& message, glm::vec3 color) {
        chat_messages.push_front({message, color});
        while (chat_messages.size() > 6) {
            chat_messages.pop_back();
        }
    }

    void HUD::add_killfeed_message(world::DrawPlayer &killer, world::DrawPlayer &victim, net::KILL kill_type) {
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

    void HUD::update_color() {
        if (!this->scene.ply) return;

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
            pos.y = (scene.client.height() - 16 * index) - 100;
            this->sys15->draw(i->message, pos, i->color, { 1, 1 });
        }

        if (this->scene.client.text_input_active()) {
            const char *prefix = this->cur_chat_type == net::CHAT::ALL ? "(Global) " : "(Team): ";
            this->sys15->draw(prefix + this->scene.client.input_buffer + "_", { 25, scene.client.height() - 100 });
        }
    }
}}