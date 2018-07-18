#include "world/player.h"

#include "common.h"
#include "game_client.h"
#include "scene/game.h"

constexpr float FALL_SLOW_DOWN = 0.24f;
constexpr float FALL_DAMAGE_VELOCITY = 0.58f;
constexpr int FALL_DAMAGE_SCALAR = 4096;

namespace ace { namespace world {
    AcePlayer::AcePlayer(scene::GameScene& scene): scene(scene), f(1, 0, 0), s(0, 1, 0), h(0, 0, 1) {
        this->mf = this->mb = this->ml = this->mr = false;
        this->jump = this->crouch = this->sneak = this->sprint = false;
        this->primary_fire = this->secondary_fire = this->weapon_equipped = false;
        this->airborne = this->wade = false;
        this->alive = true;
        this->lastclimb = 0.0;
    }

    long AcePlayer::fixed_update(double dt) {
        if(!this->alive) {
            return this->update_dead(dt);
        }
        //move player and perform simple physics (gravity, momentum, friction)
        if (this->jump)
        {
            this->jump = false;
            this->v.z = -0.36f;
        }

        float f = dt; //player acceleration scalar
        if (this->airborne)
            f *= 0.1f;
        else if (this->crouch)
            f *= 0.3f;
        else if ((this->secondary_fire && this->weapon_equipped) || this->sneak)
            f *= 0.5f;
        else if (this->sprint)
            f *= 1.3f;

        if ((this->mf || this->mb) && (this->ml || this->mr))
            f *= sqrt(0.5); //if strafe + forward/backwards then limit diagonal velocity

        if (this->mf)
        {
            this->v.x += this->f.x*f;
            this->v.y += this->f.y*f;
        }
        else if (this->mb)
        {
            this->v.x -= this->f.x*f;
            this->v.y -= this->f.y*f;
        }
        if (this->ml)
        {
            this->v.x -= this->s.x*f;
            this->v.y -= this->s.y*f;
        }
        else if (this->mr)
        {
            this->v.x += this->s.x*f;
            this->v.y += this->s.y*f;
        }

        f = dt + 1;
        this->v.z += dt;
        this->v.z /= f; //air friction
        if (this->wade)
            f = dt * 6.f + 1; //water friction
        else if (!this->airborne)
            f = dt * 4.f + 1; //ground friction
        this->v.x /= f;
        this->v.y /= f;
        float f2 = this->v.z;
        this->boxclipmove(dt);
        //hit ground... check if hurt
        if (!this->v.z && (f2 > FALL_SLOW_DOWN))
        {
            //slow down on landing
            this->v.x *= 0.5f;
            this->v.y *= 0.5f;

            //return fall damage
            if (f2 > FALL_DAMAGE_VELOCITY)
            {
                f2 -= FALL_DAMAGE_VELOCITY;
                return f2 * f2 * FALL_DAMAGE_SCALAR;
            }

            return -1; // no fall damage but play fall sound
        }

        return 0; //no fall damage
    }

    long AcePlayer::update_dead(double dt) {
        this->v.z += dt * 0.5f;

        glm::vec3 fpos = this->p;
        float f = dt * 32.f;
        this->p += this->v * f;

        if(this->scene.map.clipworld(this->p.x, this->p.y, this->p.z + 1.35f)) {
            this->v.z = -this->v.z;
            this->v *= 0.25f;
            this->p = fpos;
            return 1;
        }
        return 0;
    }

    // !this->scene.map.clipbox(
    bool AcePlayer::try_uncrouch() {
        if (!this->crouch) return true;

        float x1 = this->p.x + 0.45f;
        float x2 = this->p.x - 0.45f;
        float y1 = this->p.y + 0.45f;
        float y2 = this->p.y - 0.45f;
        float z1 = this->p.z + 2.25f;
        float z2 = this->p.z - 1.35f;

        //first check if player can lower feet (in midair)
        if (this->airborne && !(this->scene.map.clipbox(x1, y1, z1) ||
                                this->scene.map.clipbox(x1, y2, z1) ||
                                this->scene.map.clipbox(x2, y1, z1) ||
                                this->scene.map.clipbox(x2, y2, z1))) {
            this->crouch = false;
        //then check if they can raise their head
        } else if (!(this->scene.map.clipbox(x1, y1, z2) ||
              this->scene.map.clipbox(x1, y2, z2) ||
              this->scene.map.clipbox(x2, y1, z2) ||
              this->scene.map.clipbox(x2, y2, z2))) {
            this->p.z -= 0.9f;
            this->e.z -= 0.9f;
            this->crouch = false;
        }
        return !this->crouch;
    }

    void AcePlayer::boxclipmove(double dt) {
        float offset, m;
        if (this->crouch)
        {
            offset = 0.45f;
            m = 0.9f;
        }
        else
        {
            offset = 0.9f;
            m = 1.35f;
        }

        float f = dt * 32.f;
        float nx = f * this->v.x + this->p.x;
        float ny = f * this->v.y + this->p.y;
        float nz = this->p.z + offset;

        bool climb = false;
        if (this->v.x < 0) f = -0.45f;
        else f = 0.45f;
        float z = m;
        while (z >= -1.36f && !this->scene.map.clipbox(nx + f, this->p.y - 0.45f, nz + z) && !this->scene.map.clipbox(nx + f, this->p.y + 0.45f, nz + z))
            z -= 0.9f;
        if (z<-1.36f) this->p.x = nx;
        else if (!this->crouch && this->f.z<0.5f && !this->sprint)
        {
            z = 0.35f;
            while (z >= -2.36f && !this->scene.map.clipbox(nx + f, this->p.y - 0.45f, nz + z) && !this->scene.map.clipbox(nx + f, this->p.y + 0.45f, nz + z))
                z -= 0.9f;
            if (z<-2.36f)
            {
                this->p.x = nx;
                climb = true;
            }
            else this->v.x = 0;
        }
        else this->v.x = 0;

        if (this->v.y < 0) f = -0.45f;
        else f = 0.45f;
        z = m;
        while (z >= -1.36f && !this->scene.map.clipbox(this->p.x - 0.45f, ny + f, nz + z) && !this->scene.map.clipbox(this->p.x + 0.45f, ny + f, nz + z))
            z -= 0.9f;
        if (z<-1.36f) this->p.y = ny;
        else if (!this->crouch && this->f.z<0.5f && !this->sprint && !climb)
        {
            z = 0.35f;
            while (z >= -2.36f && !this->scene.map.clipbox(this->p.x - 0.45f, ny + f, nz + z) && !this->scene.map.clipbox(this->p.x + 0.45f, ny + f, nz + z))
                z -= 0.9f;
            if (z<-2.36f)
            {
                this->p.y = ny;
                climb = true;
            }
            else this->v.y = 0;
        }
        else if (!climb)
            this->v.y = 0;

        if (climb)
        {
            this->v.x *= 0.5f;
            this->v.y *= 0.5f;
            this->lastclimb = this->scene.fixed_time;
            nz--;
            m = -1.35f;
        }
        else
        {
            if (this->v.z < 0)
                m = -m;
            nz += this->v.z*dt*32.f;
        }

        this->airborne = true;

        if (this->scene.map.clipbox(this->p.x - 0.45f, this->p.y - 0.45f, nz + m) ||
            this->scene.map.clipbox(this->p.x - 0.45f, this->p.y + 0.45f, nz + m) ||
            this->scene.map.clipbox(this->p.x + 0.45f, this->p.y - 0.45f, nz + m) ||
            this->scene.map.clipbox(this->p.x + 0.45f, this->p.y + 0.45f, nz + m))
        {
            if (this->v.z >= 0)
            {
                this->wade = this->p.z > 61;
                this->airborne = false;
            }
            this->v.z = 0;
        }
        else
            this->p.z = nz - offset;

        this->reposition(dt);
    }

    void AcePlayer::set_orientation(float x, float y, float z) {
        float f = sqrtf(x*x + y * y);
        this->f = { x, y, z };
        this->s = { -y / f, x / f, 0.0 };
        this->h = { -z * this->s.y, z * this->s.x, (x * this->s.y) - (y * this->s.x) };
    }

    void AcePlayer::reposition(double dt) {
        this->e = this->p;
        double f = this->lastclimb - this->scene.fixed_time; /* FIXME meaningful name */
        if (f>-0.25f)
            this->e.z += (f + 0.25f) / 0.25f;
    }

    DrawPlayer::DrawPlayer(scene::GameScene& scene, bool local_player):
        AcePlayer(scene),
        mdl_head(scene.models.get("playerhead.kv6")),
        mdl_torso(scene.models.get("playertorso.kv6")),
        mdl_legr(scene.models.get("playerleg.kv6")),
        mdl_legl(scene.models.get("playerleg.kv6")),
        mdl_arms(scene.models.get("playerarms.kv6")),
        mdl_dead(scene.models.get("playerdead.kv6")),
        local_player(local_player),
        blocks(*this), spade(*this), grenades(*this), switch_time(0.0) {
        this->set_weapon(net::WEAPON::SEMI);
        this->set_tool(net::TOOL::WEAPON);
        this->restock(true);
    }

    const char *STEPS[] = {
        "footstep1.wav", "footstep2.wav", "footstep3.wav", "footstep4.wav",
        "wade1.wav", "wade2.wav", "wade3.wav", "wade4.wav"
    };

    void DrawPlayer::update(double dt) {
        if(!this->get_tool()->available()) {
            this->set_tool(net::TOOL(uint8_t(this->held_tool) - 1));
        }

        if (!this->alive) return;

        if (this->local_player) {
            double old_switch = this->switch_time;
            this->switch_time = std::max(old_switch - dt, 0.0);
            if(old_switch > 0 && this->switch_time <= 0.0) {
                net::SetTool st;
                st.tool = this->held_tool;
                this->scene.client.net.send_packet(st);
            }

            if (!this->scene.client.text_input_active()) {
                auto keys = this->scene.client.keyboard.keys;
                auto mouse = this->scene.client.mouse.state;
                bool changed = this->set_walk(
                    keys[this->scene.client.config.get_key("forward", SDL_SCANCODE_W)],
                    keys[this->scene.client.config.get_key("back", SDL_SCANCODE_S)],
                    keys[this->scene.client.config.get_key("left", SDL_SCANCODE_A)],
                    keys[this->scene.client.config.get_key("right", SDL_SCANCODE_D)]
                );
                changed |= this->set_animation(
                    keys[this->scene.client.config.get_key("jump", SDL_SCANCODE_SPACE)],
                    keys[this->scene.client.config.get_key("crouch", SDL_SCANCODE_LCTRL)],
                    keys[this->scene.client.config.get_key("sneak", SDL_SCANCODE_V)],
                    keys[this->scene.client.config.get_key("sprint", SDL_SCANCODE_LSHIFT)]
                );
                changed |= this->set_fire(mouse & SDL_BUTTON(SDL_BUTTON_LEFT), mouse & SDL_BUTTON(SDL_BUTTON_RIGHT));
                if (changed) this->scene.send_input_update();
            }

            if (!this->scene.thirdperson) {
                this->scene.cam.position = vox2draw(this->e);
            }
        } else {
            this->switch_time = 0.0;
        }

        if (fabs(this->v.x) > 0.01f || fabs(this->v.y) > 0.01f) {
            if (scene.time >= this->next_footstep && (mf || mb || ml || mr) && !this->airborne && !this->crouch && !this->sneak) {

                int step = this->scene.ms_time & 3;
                this->play_sound(STEPS[step + int(this->wade) * 4]);
                //                this->scene.client.sound.play(sound, vox2draw(this->p) + 0.5f, 100, this->local_player && !this->scene.thirdperson);
                this->next_footstep = scene.time + (this->sprint ? 0.386f : 0.512f);
            }
        }

        this->get_tool()->update(dt);
    }

    long DrawPlayer::fixed_update(double dt) {
        if (this->jump && !this->airborne && this->alive) {
            this->play_sound(this->wade ? "waterjump.wav" : "jump.wav");
        }

        long ret = AcePlayer::fixed_update(dt);

        if (!this->alive) {
            return -2;
        }

        if (ret == -1) {
            this->play_sound(this->wade ? "waterland.wav" : "land.wav");
        } else if (ret > 0 && this->local_player) {
            this->play_sound("fallhurt.wav");
        }

        return ret;
    }

    void DrawPlayer::set_orientation(float x, float y, float z) {
        AcePlayer::set_orientation(x, y, z);
        this->draw_forward = vox2draw(this->f); //  { this->f.x, -this->f.z, this->f.y };
        this->draw_right = glm::normalize(cross(this->scene.cam.world_up, this->draw_forward));
    }

    bool DrawPlayer::set_walk(bool mf, bool mb, bool ml, bool mr) {
        bool changed = this->mf != mf || this->mb != mb || this->ml != ml || this->mr != mr;
        this->mf = mf; this->mb = mb; this->ml = ml; this->mr = mr;
        return changed;
    }

    bool DrawPlayer::set_animation(bool jump, bool crouch, bool sneak, bool sprint) {
        bool changed = this->jump != jump || this->crouch != crouch || this->sneak != sneak || this->sprint != sprint;
        this->jump |= jump && !this->airborne; this->set_crouch(crouch); this->sneak = sneak; this->set_sprint(sprint);
        return changed;
    }

    bool DrawPlayer::set_fire(bool primary, bool secondary) {
        bool changed = this->primary_fire != primary || this->secondary_fire != secondary;
        this->primary_fire = primary && !this->sprint; this->secondary_fire = secondary && !this->sprint;
        if (changed && local_player) this->scene.set_zoom(this->secondary_fire && this->weapon_equipped);
        return changed;
    }

    void DrawPlayer::set_crouch(bool value) {
        if (this->crouch && !value) {
            this->try_uncrouch();
        }
        else {
            if (value != this->crouch && value)
                this->p.z += 0.8f;
            this->crouch = value;
        }
        this->mdl_torso.mesh = scene.models.get(this->crouch ? "playertorsoc.kv6" : "playertorso.kv6");
        this->mdl_legl.mesh = this->mdl_legr.mesh = scene.models.get(this->crouch ? "playerlegc.kv6" : "playerleg.kv6");
    }

    void DrawPlayer::set_sprint(bool value) {
        if (this->sprint && !value) {
            this->switch_time = std::max(this->switch_time, 0.3);
        }
        this->sprint = value;
    }

    void DrawPlayer::set_tool(net::TOOL tool) {
        if (!this->get_tool(tool)->available() && this->local_player) return;

        this->switch_time = 0.5f;
        if (tool == this->held_tool) return;
        
        this->weapon_equipped = tool == net::TOOL::WEAPON;
        Tool *t = this->get_tool();
        if (t) t->holster();
        this->held_tool = tool;
        t = this->get_tool();
        if(t) t->deploy();
        
        if(this->local_player) {
            this->play_sound("switch.wav");
            this->scene.hud.update_tool(t->ammo_icon());
        }
    }

    void DrawPlayer::set_weapon(net::WEAPON weapon) {
        if (weapon == this->equipped_weapon) return;

        this->equipped_weapon = weapon;
        switch (this->equipped_weapon) {
        case net::WEAPON::SEMI: this->weapon_obj = std::make_unique<SemiWeapon>(*this); break;
        case net::WEAPON::SMG: this->weapon_obj = std::make_unique<SMGWeapon>(*this); break;
        case net::WEAPON::SHOTGUN: this->weapon_obj = std::make_unique<ShotgunWeapon>(*this); break;
        default: break;
        }
        this->weapon_obj->restock(true);

        if(this->local_player) {
            this->scene.hud.update_weapon(this->weapon_obj->sight());
        }
    }

    void DrawPlayer::set_color(glm::u8vec3 color) {
        this->color = color;
        if(this->local_player) {
            net::SetColor sc;
            sc.color = color;
            this->scene.client.net.send_packet(sc);
        }
    }

    void DrawPlayer::set_alive(bool value) {
        this->alive = value;

        if (this->alive) {
            this->restock(true);
        } else {
            if(this->local_player) {
                this->play_sound("death.wav");
                this->play_sound("hitplayer.wav");
            }
            this->health = 0;
        }
    }

    void DrawPlayer::draw() {
        this->transform();
        auto tool = this->get_tool();
        tool->transform();

        scene.shaders.model.uniform("replacement_color", this->scene.teams[this->team].float_color * 0.5f);
        if(!this->alive) {
            if(!local_player) this->mdl_dead.draw(this->scene.shaders.model);
            return;
        }

        if(!local_player || scene.thirdperson) {
            this->mdl_head.draw(this->scene.shaders.model);
            this->mdl_torso.draw(this->scene.shaders.model);
            this->mdl_legl.draw(this->scene.shaders.model);
            this->mdl_legr.draw(this->scene.shaders.model);
            this->mdl_arms.draw(this->scene.shaders.model);
            tool->draw();
        } else if(!this->sprint && this->switch_time < 0.5f && this->get_tool()->drawable() && !(this->secondary_fire && this->weapon_equipped)) {
            this->mdl_arms.draw_local(this->scene.shaders.model);
            tool->draw();
        }
    }

    void DrawPlayer::set_position(float x, float y, float z) {
        this->p = this->e = { x, y, z };
;    }

    Tool *DrawPlayer::get_tool(net::TOOL tool) {
        if (tool == net::TOOL::INVALID) tool = this->held_tool;

        switch(tool) {
        case net::TOOL::BLOCK: return &this->blocks;
        case net::TOOL::SPADE: return &this->spade;
        case net::TOOL::WEAPON: return this->weapon_obj.get();
        case net::TOOL::GRENADE: return &this->grenades;
        default: return &this->spade;
        }
    }

    void DrawPlayer::transform() {
        const glm::vec2 angles = dir2ang(this->draw_forward);
        const float yaw = angles.x, pitch = angles.y;

        if (!this->alive) {
            this->mdl_dead.rotation = { 0, -yaw + 90, 0 };
            this->mdl_dead.position = { this->p.x, -this->p.z - 1.25f, this->p.y };
            return;
        }

        float c = cos(glm::radians(-yaw + 90));
        float s = sin(glm::radians(-yaw + 90));

        if (!this->local_player || this->scene.thirdperson) {
            this->mdl_head.position = this->mdl_torso.position = { this->e.x, -this->e.z - 0.3f, this->e.y };
            this->mdl_arms.position = { this->e.x, -this->e.z - (crouch ? 0.4f : 0.5f), this->e.y };
            //            this->mdl_arms.position += draw_forward * bob + scene.cam.up * (airborne ? v.z * 0.2f : 0);
            float legxy_mod = crouch ? -0.3f : 0.f;
            this->mdl_legr.position = { this->e.x + c * 0.25f + legxy_mod * s, -this->e.z - (crouch ? 0.7f : 1.1f), this->e.y - s * 0.25f + legxy_mod * c };
            this->mdl_legl.position = { this->e.x - c * 0.25f + legxy_mod * s, -this->e.z - (crouch ? 0.7f : 1.1f), this->e.y + s * 0.25f + legxy_mod * c };

            this->mdl_head.rotation = this->mdl_arms.rotation = glm::vec3(-pitch, -yaw + 90, 0);
            this->mdl_torso.rotation = this->mdl_legl.rotation = this->mdl_legr.rotation = { 0, -yaw + 90, 0 };

            if (this->sprint) {
                this->mdl_arms.rotation.x = 50;
            }

            if (fabs(v.x) > 0.01f || fabs(v.y) > 0.01f) {
                float rot1, rot2;
                rot1 = rot2 = (this->scene.ms_time & 511) - 255.f;

                rot1 *= this->f.x * this->v.x + this->f.y * this->v.y * 0.028f * 30;
                rot2 *= this->s.x * this->v.x + this->s.y * this->v.y * 0.028f * 30;

                //            rot1 *= 0.028;
                //            rot2 *= 0.028;
                //            rot1 *= 30;
                //            rot2 *= 30;
                if ((this->scene.ms_time & 1023) > 511) {
                    this->mdl_legl.rotation.x = -rot1;
                    this->mdl_legl.rotation.z = -rot2;
                    this->mdl_legr.rotation.x = rot1;
                    this->mdl_legr.rotation.z = rot2;
                }
                else {
                    this->mdl_legl.rotation.x = rot1;
                    this->mdl_legl.rotation.z = rot2;
                    this->mdl_legr.rotation.x = -rot1;
                    this->mdl_legr.rotation.z = -rot2;
                }
            }
        } else {
            float bob = ((scene.ms_time & 511) - 255.f) * std::max(std::fabs(v.x), std::fabs(v.y)) / 1000.f;
            this->mdl_arms.position = { -0.01f, -0.45f + (v.z * 0.2f * airborne), 0.1f + (scene.ms_time & 1023) > 511 ? bob : -bob };
            this->mdl_arms.rotation = { 0, 0, 0 };
            this->mdl_arms.lighting_rotation = glm::vec3(-pitch, -yaw + 90, 0); // because lighting haHAA

            float t = this->switch_time * 5;
            this->mdl_arms.position -= glm::vec3{ -t, t, 0 };
        }
    }

    void DrawPlayer::restock(bool primary) {
        this->blocks.restock(primary);
        this->spade.restock(primary);
        if(this->weapon_obj)
            this->weapon_obj->restock(primary);
        this->grenades.restock(primary);
        this->health = 100;

        this->play_sound("switch.wav");
    }

    void DrawPlayer::play_sound(const std::string &name, int volume) const {
        this->scene.client.sound.play(name, vox2draw(this->p), 100, this->local_player && !this->scene.thirdperson);
    }
}}