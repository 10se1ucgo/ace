#include "weapon.h"

#include "glm/gtx/norm.hpp"

#include "scene/game.h"
#include "world/player.h"
#include "world/tracer.h"
#include "world/grenade.h"
#include "world/debris.h"
#include "draw/map.h"
#include <glm/gtx/string_cast.hpp>


namespace ace {
    Tool::Tool(world::DrawPlayer& ply) : ply(ply) {

    }

    void Tool::update(double dt) {
        if (this->ply.sprint || this->ply.switch_time > 0) return;

        if (this->ply.primary_fire && this->primary_rate() > 0.f) {
            if ((this->primary_ammo > 0 || this->max_primary() < 0) && this->ply.scene.time >= this->next_primary) {
                if (this->on_primary())
                    this->next_primary = this->ply.scene.time + this->primary_rate();
            }
        } else if (this->ply.secondary_fire && this->secondary_rate() > 0.f) {
            if ((this->secondary_ammo > 0 || this->max_secondary() < 0) && this->ply.scene.time >= this->next_secondary) {
                if (this->on_secondary())
                    this->next_secondary = this->ply.scene.time + this->secondary_rate();
            }
        }
    }

    SpadeTool::SpadeTool(world::DrawPlayer& ply) : Tool(ply), mdl(ply.scene.models.get("spade.kv6"), 0.05f) {
        this->last_secondary = this->ply.secondary_fire;
    }

    std::string SpadeTool::display_ammo() { return this->ply.blocks.display_ammo(); }

    std::string SpadeTool::ammo_icon() { return this->ply.blocks.ammo_icon(); }

    void SpadeTool::update(double dt) {
        if (this->ply.secondary_fire && !this->last_secondary) {
            this->next_secondary = this->ply.scene.time + this->secondary_rate();
        }
        this->last_secondary = this->ply.secondary_fire;
        Tool::update(dt);
    }

    void SpadeTool::draw() {
        if (!this->ply.local_player || this->ply.scene.thirdperson) {
            this->mdl.draw(this->ply.scene.cam.matrix(), this->ply.scene.shaders.model);
        }
        else {
            this->mdl.draw_local(this->ply.scene.cam.projection, this->ply.scene.shaders.model);
        }
    }

    bool SpadeTool::on_primary() {
        this->spade(false);
        return true;
    }

    bool SpadeTool::on_secondary() {
        this->spade(true);
        return true;
    }

    void SpadeTool::spade(bool secondary) {
        glm::ivec3 hit;
        Face f = this->ply.scene.map.hitscan(this->ply.e, this->ply.f, &hit);

        if (f == Face::INVALID || !all(lessThan(glm::abs(this->ply.p - glm::vec3(hit)), glm::vec3(3.5f)))) {
            return this->ply.play_sound("woosh.wav");;
        }

        if (secondary) {
            if(this->ply.local_player) this->ply.scene.destroy_point(hit.x, hit.y, hit.z, net::ACTION::SPADE);
        } else {
            this->ply.scene.damage_point(hit.x, hit.y, hit.z, this->ply.local_player ? 64 : 0, f);
        }
        this->ply.play_sound("hitground.wav");
    }

    void SpadeTool::transform() {
        float transform = this->ply.scene.time - this->next_primary;

        if (transform < 0)
            this->ply.mdl_arms.rotation.x -= transform * 180;

        this->mdl.position = this->ply.mdl_arms.position + ang2dir(-(this->ply.mdl_arms.rotation.y - 90), -this->ply.mdl_arms.rotation.x) * 0.875f + this->ply.draw_right * -0.4f + ply.scene.cam.up * .125f;
        this->mdl.rotation = this->ply.mdl_arms.rotation;



        if (!this->ply.local_player) return;


        if (transform < 0)
            this->ply.mdl_arms.local_rotation.x += -transform * 490;

        this->mdl.local_position = this->ply.mdl_arms.local_position + glm::vec3(0.4f, 0.125f, -0.9f);
        this->mdl.local_rotation = this->ply.mdl_arms.local_rotation;
    }

    BlockTool::BlockTool(world::DrawPlayer& ply) : Tool(ply), mdl(ply.scene.models.get("block.kv6"), 0.065f), m1(-1), m2(-1) {
        this->mdl.local_rotation.y = 1;
        this->mdl.local_scale = glm::vec3(0.05f);
        this->last_secondary = this->ply.secondary_fire;
    }

    void BlockTool::update(double dt) {
        if (this->ply.local_player) {
            glm::ivec3 hit;
            Face face = this->ply.scene.map.hitscan(this->ply.p, this->ply.f, &hit);
            hit = draw::DrawMap::next_block(hit.x, hit.y, hit.z, face);



            if(!this->ply.secondary_fire) {
                if(this->last_secondary) {
                    this->ply.scene.send_block_line(this->m1, this->m2);
                    this->m1 = this->m2 = hit;
                    this->ghost_block_line(); // revert ghost block to single block (inefficient i know)
                }
                this->m1 = hit;
            } else {
                if (this->m2 != hit) {
                    this->m2 = hit;
                    this->ghost_block_line();
                }
            }

            this->last_secondary = this->ply.secondary_fire;
        }
        Tool::update(dt);
    }

    void BlockTool::draw() {
        this->ply.scene.shaders.model.uniform("replacement_color", glm::vec3(this->ply.color) / 255.f);

        if (!this->ply.local_player || this->ply.scene.thirdperson) {
            this->mdl.draw(this->ply.scene.cam.matrix(), this->ply.scene.shaders.model);
        }
        else {
            this->mdl.draw_local(this->ply.scene.cam.projection, this->ply.scene.shaders.model);
        }


        if (!this->ply.local_player) return;

        this->ply.scene.shaders.map.bind();
        this->ply.scene.shaders.map.uniform("alpha", 0.5f);
        this->ply.scene.shaders.map.uniform("replacement_color", glm::vec3{ this->ply.color } / 255.f);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        this->ghost_block->draw(this->ply.scene.cam.matrix(), this->ply.scene.shaders.map);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        this->ply.scene.shaders.model.bind();
    }


    bool BlockTool::on_primary() {

        if (!this->ply.local_player) return true;

        glm::ivec3 hit;
        Face face = this->ply.scene.map.hitscan(this->ply.e, this->ply.f, &hit);
        if (face == Face::INVALID) {
            return false;
        }
        hit = draw::DrawMap::next_block(hit.x, hit.y, hit.z, face);
        this->ply.scene.build_point(hit.x, hit.y, hit.z, this->ply.color, false);
        this->ply.play_sound("switch.wav", 50);
        //    auto dir = rand_normalized();
        //    auto angs = dir2ang(dir);
        //    fmt::print("DIR: {}, DIR->ANG(): {}, DIR->ANG->DIR(): {}", to_string(dir), to_string(angs), to_string(ang2dir(-angs.y, -angs.x + 90)));


        return true;
    }

    bool BlockTool::on_secondary() {
        return true;

//        if (!this->ply.local_player) return true;
//
//
//        glm::ivec3 hit;
//        if (this->ply.scene.map.hitscan(this->ply.p, this->ply.f, &hit) != Face::INVALID) {
//            uint8_t a, r, g, b;
//            unpack_color(this->ply.scene.map.get_color(hit.x, hit.y, hit.z), &a, &r, &g, &b);
//            this->ply.set_color({ r, g, b });
//            this->ply.scene.hud.palret.position = glm::vec2(-50, -50);
//
//            //        this->ply.scene.destroy_point(hit.x, hit.y, hit.z);
//        }
//        return true;
    }

    void BlockTool::ghost_block_line() {
        if (!this->ply.local_player) return;

        auto blocks(this->ply.scene.map.block_line(this->m1, this->m2));
        std::vector<VXLBlock> d_blocks;
        for(auto &x : blocks) {
            d_blocks.push_back({x, 0xFF000000});
        }
        this->ghost_block->update(d_blocks, this->m2, true);
    }

    void BlockTool::transform() {
        float transform = this->ply.scene.time - this->next_primary;

        if (transform < 0)
            this->ply.mdl_arms.rotation.x -= transform * 60;

        this->mdl.position = this->ply.mdl_arms.position + ang2dir(-(this->ply.mdl_arms.rotation.y - 90), -this->ply.mdl_arms.rotation.x) + this->ply.draw_right * -0.4f;
        this->mdl.rotation = this->ply.mdl_arms.rotation;


        if (!this->ply.local_player) return;


        if (transform < 0) {
            this->ply.mdl_arms.local_position += glm::vec3{ -transform, transform, 0 };
        }

        this->mdl.local_position = this->ply.mdl_arms.local_position + glm::vec3(0.4f, 0.0f, -1.0f);

        if (!this->ghost_block) {
            this->ghost_block = std::make_unique<draw::VXLBlocks>(std::vector<VXLBlock>{ VXLBlock{ { 0, 0, 0 }, 0xFF000000, 0b11111111 } });
        }

        this->ghost_block->position = vox2draw(this->ply.secondary_fire ? this->m2 : this->m1);
    }

    GrenadeTool::GrenadeTool(world::DrawPlayer& ply) : Tool(ply), mdl(ply.scene.models.get("grenade.kv6"), 0.05f) {
        this->deploy();
    }

    void GrenadeTool::update(double dt) {
        if (this->primary_ammo <= 0) return;

        if ((!this->ply.primary_fire && last_primary) || this->fuse <= 0) {
            this->on_primary();
            this->fuse = MAX_FUSE;
        }
        if (this->ply.primary_fire && !last_primary) {
            this->ply.play_sound("pin.wav");
        }
        last_primary = this->ply.primary_fire;
        if (this->ply.primary_fire) {
            fuse -= dt;
        }
    }

    void GrenadeTool::draw() {
        if (!this->ply.local_player || this->ply.scene.thirdperson) {
            this->mdl.draw(this->ply.scene.cam.matrix(), this->ply.scene.shaders.model);
        }
        else {
            this->mdl.draw_local(this->ply.scene.cam.projection, this->ply.scene.shaders.model);
        }
    }

    bool GrenadeTool::on_primary() {
        this->ply.play_sound("woosh.wav");
        if (!this->ply.local_player) return true;

        this->ply.scene.send_grenade(this->fuse);
        this->ply.scene.create_object<world::Grenade>(this->ply.p, this->ply.f + this->ply.v, this->fuse);
        this->primary_ammo--;
        return true;
    }

    void GrenadeTool::deploy() {
        this->last_primary = this->ply.primary_fire;
        this->fuse = MAX_FUSE;
    }

    void GrenadeTool::transform() {
        float transform = fuse - MAX_FUSE;

        if (transform < 0)
            this->ply.mdl_arms.rotation.x += transform * 30;

        this->mdl.position = this->ply.mdl_arms.position + ang2dir(-(this->ply.mdl_arms.rotation.y - 90), -this->ply.mdl_arms.rotation.x) * 0.875f + this->ply.draw_right * -0.4f;
        this->mdl.rotation = this->ply.mdl_arms.rotation;

        if (!this->ply.local_player) return;

        if (transform < 0) {
            this->ply.mdl_arms.local_position += glm::vec3{ transform * -1.5, transform * 0.5, 0 };
            //        this->ply.mdl_arms.local_rotation.x += transform * 40;
        }

        this->mdl.local_position = this->ply.mdl_arms.local_position + glm::vec3(0.4f, 0.125f, -0.9f);;
        this->mdl.local_rotation = this->ply.mdl_arms.local_rotation;
    }

    Weapon::Weapon(world::DrawPlayer& ply, const std::string &model) : Tool(ply), mdl(ply.scene.models.get(model), 0.05f) {
    }

    void Weapon::update(double dt) {
        Tool::update(dt);
    }

    void Weapon::draw() {
        if (!this->ply.local_player || this->ply.scene.thirdperson) {
            this->mdl.draw(this->ply.scene.cam.matrix(), this->ply.scene.shaders.model);
        }
        else {
            this->mdl.draw_local(this->ply.scene.cam.projection, this->ply.scene.shaders.model);
        }
    }

    bool Weapon::reload() {
        if (!this->ply.local_player) return true;

        if (this->secondary_ammo == 0 || this->primary_ammo == this->max_primary())
            return false;

                                                // whatever data we send is ignored anyways
        this->ply.scene.client.net.send_packet(net::PACKET::WeaponReload, net::WeaponReload{});

        if (!this->one_by_one() && !this->reloading)
            this->ply.play_sound(this->reload_sound());

        this->reloading = true;

        return true;
    }

    void Weapon::on_reload(net::WeaponReload *pkt) {
        if (pkt == nullptr) {
            this->restock(true);
        } else {
            this->primary_ammo = pkt->primary;
            this->secondary_ammo = pkt->secondary;
        }

        if (this->one_by_one()) {
            this->reloading = this->primary_ammo != this->max_primary() && this->secondary_ammo > 0;
            const std::string sound = this->reloading ? this->reload_sound() : "cock.wav"; // todo `virtual std::string reload_done_sound();` or smth
            this->ply.play_sound(sound);
        }
        else {
            this->reloading = false;
            if (!this->ply.local_player) {
                this->ply.play_sound(this->reload_sound());
            }
        }
    }

    namespace {
        // thanks br[azil] (again)
        float calc_spread(float max_spread) {
            // ReSharper disable once CppIdenticalOperandsInBinaryExpression
            // screw off no its not
            return (random::random() - random::random()) * max_spread;
        }
    }

    bool Weapon::on_primary() {
        if (this->one_by_one()) {
            this->reloading = false;
        }

        if (this->reloading || this->primary_ammo <= 0) {
            return false;
        }

        this->ply.play_sound(this->shoot_sound());

        // stupid PYSPADES bans you if you spam BlockAction more than once per shot (shotgun creates problems)
        bool block_destroyed = false;
        for (int i = 0; i < this->pellets(); i++) {
            glm::vec3 dir = this->ply.f;
            float spread = this->spread() * (this->ply.secondary_fire ? 1 : 2);
            dir += glm::vec3{ calc_spread(spread), calc_spread(spread), calc_spread(spread) };
            this->ply.scene.create_object<world::Tracer>(this->tracer(), this->ply.e + dir * 4.f, dir);

            glm::ivec3 hit;
            Face f = this->ply.scene.map.hitscan(this->ply.e, dir, &hit);
            if (f != Face::INVALID) {
                // damage 0 if != local_ply just to have particles
                this->ply.scene.client.sound.play("impact.wav", vox2draw(hit) + .5f, 50);
                block_destroyed |= this->ply.scene.damage_point(hit.x, hit.y, hit.z, this->ply.local_player ? this->block_damage() : 0, f, !block_destroyed);
            }


            for (auto &kv : this->ply.scene.players) {
                if (!kv.second->alive || kv.second.get() == &this->ply) continue;

                glm::vec3 h;
                net::HIT type;

                if (kv.second->mdl_head.sprhitscan(this->ply.e, dir, &h))
                    type = net::HIT::HEAD;
                else if (kv.second->mdl_torso.sprhitscan(this->ply.e, dir, &h))
                    type = net::HIT::TORSO;
                else if (kv.second->mdl_legl.sprhitscan(this->ply.e, dir, &h))
                    type = net::HIT::LEGS;
                else if (kv.second->mdl_legr.sprhitscan(this->ply.e, dir, &h))
                    type = net::HIT::LEGS;
                else
                    continue;

                // TODO: prevent being able to shoot through blocks

                this->ply.scene.create_object<world::DebrisGroup>(h, glm::vec3{ 127, 0, 0 }, 0.25f, 4);
                if (this->ply.local_player) {
                    net::HitPacket hp;
                    hp.pid = kv.second->pid;
                    hp.value = type;
                    this->ply.scene.client.net.send_packet(net::PACKET::HitPacket, hp);
                    break;
                }
            }
        }

        this->primary_ammo--;

        if (!this->ply.local_player || this->ply.scene.thirdperson)
            return true;

        auto recoil = this->recoil();

        auto time = this->ply.scene.ms_time;

        float yaw = ((time & 511) - 255.5) * ((time & 1023) > 511 ? recoil.y : -recoil.y);
        float pitch = recoil.x;

        if ((this->ply.mf || this->ply.mb || this->ply.ml || this->ply.mr) && !this->ply.crouch) {
            yaw *= 2; pitch *= 2;
        }
        if (this->ply.airborne) {
            yaw *= 2; pitch *= 2;
        }
        else if (this->ply.crouch) {
            yaw /= 2; pitch /= 2;
        }

        this->ply.scene.cam.yaw += yaw * 60;
        this->ply.scene.cam.pitch -= pitch * 60;
        return true;
    }

    void Weapon::transform() {
        float transform = this->ply.scene.time - this->next_primary;

        if (transform < 0)
            this->ply.mdl_arms.rotation.x += transform * 45;

        this->mdl.position = this->ply.mdl_arms.position + ang2dir(-(this->ply.mdl_arms.rotation.y - 90), -this->ply.mdl_arms.rotation.x) + this->ply.draw_right * -0.4f;
        this->mdl.rotation = this->ply.mdl_arms.rotation;

        if (!this->ply.local_player) return;


        if (transform < 0) {
            float f = transform / 32;
            float f2 = f * 10;
            this->ply.mdl_arms.local_position += glm::vec3{ -f2, f2, 0 };
            // this->ply.mdl_arms.local_rotation.x += f * 280;
        }

        this->mdl.local_position = this->ply.mdl_arms.local_position + glm::vec3(0.4, 0, -1.0f);
        this->mdl.local_rotation = this->ply.mdl_arms.local_rotation;
    }
}
