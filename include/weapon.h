#pragma once
#include <string>

#include "draw/kv6.h"
#include "draw/map.h"

namespace ace {
    namespace world { struct DrawPlayer; }

    struct Tool {
        Tool(world::DrawPlayer &ply);
        virtual ~Tool() = default;

        virtual std::string name() const = 0;

        virtual std::string display_ammo() const { return std::to_string(this->primary_ammo) + "-" +  std::to_string(this->secondary_ammo); }
        virtual std::string ammo_icon() const = 0;

        virtual int max_primary() const = 0;
        virtual int max_secondary() const = 0;

        virtual float primary_rate() const = 0;
        virtual float secondary_rate() const = 0;
        virtual float use_rate() const { return 0.0f; }

        virtual bool on_primary() { return true; }
        virtual bool on_secondary() { return true; }
        virtual bool on_use() { return true; }

        virtual bool can_primary();
        virtual bool can_secondary();
        virtual bool can_use();

        virtual void update(double dt);
        virtual void draw() = 0;
        virtual void transform() = 0;

        virtual void deploy() {}
        virtual void holster() {}

        virtual bool reload() { return true; }
        virtual void on_reload(net::WeaponReload *pkt) { };

        virtual void restock(bool primary = false) {
            this->secondary_ammo = this->max_secondary();
            if(primary) {
                this->primary_ammo = this->max_primary();
            }
        }

        virtual bool available() { return this->primary_ammo > 0 || this->secondary_ammo > 0; }
        virtual bool drawable() { return this->available(); }

        float next_primary = 0, next_secondary = 0, next_use = 0;
        int primary_ammo = 0, secondary_ammo = 0;
    
        world::DrawPlayer &ply;
    };

    struct SpadeTool final : Tool {
        SpadeTool(world::DrawPlayer& ply);

        std::string name() const final { return "Spade"; }

        std::string display_ammo() const final;
        std::string ammo_icon() const final;

        int max_primary() const final { return -1; }
        int max_secondary() const final { return -1; }

        float primary_rate() const final { return 0.2f; }
        float secondary_rate() const final { return 1.0f; }

        void restock(bool primary = false) final {}

        void update(double dt) final;
        void draw() final;
        void transform() final;

        bool available() final { return true; }

        bool on_primary() final;
        bool on_secondary() final;

        void spade(bool secondary);

        draw::KV6 mdl;
        bool last_secondary;
    };

    struct BlockTool final : Tool {
        BlockTool(world::DrawPlayer& ply);

        std::string name() const final { return "Block"; }

        std::string display_ammo() const final { return std::to_string(this->primary_ammo); };
        std::string ammo_icon() const final { return "block.bmp"; }

        int max_primary() const final { return 50; }
        int max_secondary() const final { return 1; }

        float primary_rate() const final { return 0.5f; }
        float secondary_rate() const final { return -1; }
        float use_rate() const final { return 0.5f; }

        void restock(bool primary = false) final { Tool::restock(true); }

        void update(double dt) final;
        void draw() final;
        void transform() final;

        bool available() final { return this->primary_ammo > 0; }

        bool on_primary() final;
        bool on_secondary() final;
        bool on_use() final;

        void ghost_block_line();

        draw::KV6 mdl;
        std::unique_ptr<draw::VXLBlocks> ghost_block;
        glm::ivec3 m1, m2;
        bool last_secondary;
    };

    struct GrenadeTool final : Tool {
        GrenadeTool(world::DrawPlayer& ply);

        std::string name() const final { return "Grenade"; }

        std::string display_ammo() const final { return std::to_string(this->primary_ammo); };
        std::string ammo_icon() const final { return "grenade.bmp"; }

        int max_primary() const final { return 3; }
        int max_secondary() const final { return 0; }

        float primary_rate() const final { return -1; }
        float secondary_rate() const final { return -1; }

        void restock(bool primary = false) final { Tool::restock(true); }

        void update(double dt) final;
        void draw() final;
        void transform() final;

        bool on_primary() final;

        void deploy() final;

        draw::KV6 mdl;
        bool last_primary;
        float fuse;
        constexpr static float MAX_FUSE = 3.f;
    };

    struct Weapon : Tool {
        explicit Weapon(world::DrawPlayer& ply, const std::string &model);

        void update(double dt) final;
        void draw() final;
        void transform() final;

        virtual std::string sight() const = 0;

        virtual std::string model() const = 0;
        virtual std::string tracer() const = 0;
        virtual std::string casing() const = 0;

        virtual std::string shoot_sound() const = 0;
        virtual std::string reload_sound() const = 0;

        float secondary_rate() const final { return 0.0f; }

        virtual float reload_time() = 0;

        virtual bool one_by_one() { return false; }
        virtual int pellets() { return 1; }

        virtual uint8_t block_damage() { return 64; }

        virtual glm::vec2 recoil() = 0;
        virtual float spread() = 0;

        void restock(bool primary = false) final { Tool::restock(primary); this->reloading &= !primary; }
        bool reload() final;

        void on_reload(net::WeaponReload *pkt) final;
        bool on_primary() override;

        bool drawable() final { return this->available() && !this->reloading; }

        bool reloading = false;

        draw::KV6 mdl/*, mdl_casing*/;
    };

    struct SemiWeapon final : Weapon {
        explicit SemiWeapon(world::DrawPlayer& ply) : Weapon(ply, this->model()) {
        }

        std::string name() const final { return "Rifle"; }
        std::string sight() const final { return "semi.png"; }
        std::string ammo_icon() const final { return "semi.bmp"; }

        std::string model() const final { return "semi.kv6"; }
        std::string tracer() const final { return "semitracer.kv6"; }
        std::string casing() const final { return "semicasing.kv6"; }

        std::string shoot_sound() const final { return "semishoot.wav"; }
        std::string reload_sound() const final { return "semireload.wav"; }

        int max_primary() const final { return 10; }
        int max_secondary() const final { return 50; }

        float primary_rate() const final { return 0.5f; }

        float reload_time() final { return 2.5f; }

        glm::vec2 recoil() final { return { -0.05, 0.0001f }; }
        float spread() final { return 0.003f; }
    };


    struct SMGWeapon final : Weapon {
        explicit SMGWeapon(world::DrawPlayer& ply) : Weapon(ply, this->model()) {
        }

        std::string name() const final { return "SMG"; }
        std::string sight() const final { return "smg.png"; }
        std::string ammo_icon() const final { return "smg.bmp"; }

        std::string model() const final { return "smg.kv6"; }
        std::string tracer() const final { return "smgtracer.kv6"; }
        std::string casing() const final { return "smgcasing.kv6"; }

        std::string shoot_sound() const final { return "smgshoot.wav"; }
        std::string reload_sound() const final { return "smgreload.wav"; }

        int max_primary() const final { return 30; }
        int max_secondary() const final { return 120; }

        float primary_rate() const final { return 0.115f; }

        uint8_t block_damage() final { return 48; }

        float reload_time() final { return 2.5f; }

        glm::vec2 recoil() final { return { -0.0125, 0.00005f }; }
        float spread() final { return 0.006f; }
    };


    struct ShotgunWeapon final : Weapon {
        explicit ShotgunWeapon(world::DrawPlayer& ply) : Weapon(ply, this->model()) {
        }

        bool one_by_one() final { return true; }
        int pellets() final { return 8; }

        std::string name() const final { return "Shotgun"; }
        std::string sight() const final { return "shotgun.png"; }
        std::string ammo_icon() const final { return "shotgun.bmp"; }

        std::string model() const final { return "shotgun.kv6"; }
        std::string tracer() const final { return "shotguntracer.kv6"; }
        std::string casing() const final { return "shotguncasing.kv6"; }

        std::string shoot_sound() const final { return "shotgunshoot.wav"; }
        std::string reload_sound() const final { return "shotgunreload.wav"; }

        int max_primary() const final { return 6; }
        int max_secondary() const final { return 48; }

        float primary_rate() const final { return 1.0f; }

        uint8_t block_damage() final { return 48; }

        float reload_time() final { return 0.5f; }
        glm::vec2 recoil() final { return { -0.1, 0.0002f }; }
        float spread() final { return 0.012f; }
    };
}
