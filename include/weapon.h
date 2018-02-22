#pragma once
#include <string>
#include "kv6.h"
#include "draw/map.h"

namespace net {
    struct WeaponReload;
}

namespace ace { namespace world {
    struct DrawPlayer;
}}

struct Tool {
    Tool(ace::world::DrawPlayer &ply);
    virtual ~Tool() = default;

    virtual std::string name() = 0;

    virtual std::string display_ammo() { return std::to_string(this->primary_ammo) + "-" +  std::to_string(this->secondary_ammo); }
    virtual std::string ammo_icon() = 0;

    virtual int max_primary() = 0;
    virtual int max_secondary() = 0;

    virtual float primary_rate() = 0;
    virtual float secondary_rate() = 0;
    virtual float use_rate() { return 0.0f; }

    virtual void update(double dt);
    virtual void draw() = 0;

    virtual bool on_primary() { return true; }
    virtual bool on_secondary() { return true; }

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
    
    ace::world::DrawPlayer &ply;
};

struct SpadeTool : Tool {
    SpadeTool(ace::world::DrawPlayer& ply);

    std::string name() final { return "Spade"; }

    std::string display_ammo() final;
    std::string ammo_icon() final;

    int max_primary() final { return -1; }
    int max_secondary() final { return -1; }

    float primary_rate() final { return 0.2f; }
    float secondary_rate() final { return 1.0f; }

    void restock(bool primary = false) final {}

    void update(double dt) final;
    void draw() final;

    bool available() final { return true; }

    bool on_primary() final;
    bool on_secondary() final;

    KV6 mdl;
    bool last_secondary;
};

struct BlockTool : Tool {
    BlockTool(ace::world::DrawPlayer& ply);

    std::string name() final { return "Block"; }

    std::string display_ammo() final { return std::to_string(this->primary_ammo); };
    std::string ammo_icon() final { return "block.bmp"; }

    int max_primary() final { return 50; }
    int max_secondary() final { return 1; }

    float primary_rate() final { return 0.5f; }
    float secondary_rate() final { return 0.1f; }

    void restock(bool primary = false) final { Tool::restock(true); }

    void update(double dt) final;
    void draw() final;

    bool available() final { return this->primary_ammo > 0; }

    bool on_primary() final;
    bool on_secondary() final;

    KV6 mdl;
    std::unique_ptr<ace::draw::VXLBlocks> ghost_block;
};

struct GrenadeTool : Tool {
    GrenadeTool(ace::world::DrawPlayer& ply);

    std::string name() final { return "Grenade"; }

    std::string display_ammo() final { return std::to_string(this->primary_ammo); };
    std::string ammo_icon() final { return "grenade.bmp"; }

    int max_primary() final { return 3; }
    int max_secondary() final { return 0; }

    float primary_rate() final { return 0.2f; }
    float secondary_rate() final { return 0.0f; }

    void restock(bool primary = false) final { Tool::restock(true); }

    void update(double dt) final;
    void draw() final;

    bool on_primary() final;


    KV6 mdl;
    bool last_primary;
    float fuse;
    constexpr static float MAX_FUSE = 3.f;
};



struct Weapon : Tool {
    explicit Weapon(ace::world::DrawPlayer& ply, const std::string &model);

    void update(double dt) final;
    void draw() final;

    virtual std::string sight() = 0;

    virtual std::string model() = 0;
    virtual std::string tracer() = 0;

    virtual std::string shoot_sound() = 0;
    virtual std::string reload_sound() = 0;

    float secondary_rate() final { return 0.0f; }

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

    KV6 mdl;
};

struct SemiWeapon : Weapon {
    explicit SemiWeapon(ace::world::DrawPlayer& ply) : Weapon(ply, this->model()) {
    }

    std::string name() final { return "Rifle"; }
    std::string sight() final { return "semi.png"; }
    std::string ammo_icon() final { return "semi.bmp"; }

    std::string model() final { return "semi.kv6"; }
    std::string tracer() final { return "semitracer.kv6"; }

    std::string shoot_sound() final { return "semishoot.wav"; }
    std::string reload_sound() final { return "semireload.wav"; }

    int max_primary() final { return 10; }
    int max_secondary() final { return 50; }

    float primary_rate() final { return 0.5f; }

    float reload_time() final { return 2.5f; }

    glm::vec2 recoil() final { return { -0.05, 0.0001f }; }
    float spread() final { return 0.003f; }
};


struct SMGWeapon : Weapon {
    explicit SMGWeapon(ace::world::DrawPlayer& ply) : Weapon(ply, this->model()) {
    }

    std::string name() final { return "SMG"; }
    std::string sight() final { return "smg.png"; }
    std::string ammo_icon() final { return "smg.bmp"; }

    std::string model() final { return "smg.kv6"; }
    std::string tracer() final { return "smgtracer.kv6"; }

    std::string shoot_sound() final { return "smgshoot.wav"; }
    std::string reload_sound() final { return "smgreload.wav"; }

    int max_primary() final { return 30; }
    int max_secondary() final { return 120; }

    float primary_rate() final { return 0.115f; }

    uint8_t block_damage() final { return 48; }

    float reload_time() final { return 2.5f; }

    glm::vec2 recoil() final { return { -0.0125, 0.00005f }; }
    float spread() final { return 0.006f; }
};


struct ShotgunWeapon : Weapon {
    explicit ShotgunWeapon(ace::world::DrawPlayer& ply) : Weapon(ply, this->model()) {
    }

    bool one_by_one() final { return true; }
    int pellets() final { return 8; }

    std::string name() final { return "Shotgun"; }
    std::string sight() final { return "shotgun.png"; }
    std::string ammo_icon() final { return "shotgun.bmp"; }

    std::string model() final { return "shotgun.kv6"; }
    std::string tracer() final { return "shotguntracer.kv6"; }

    std::string shoot_sound() final { return "shotgunshoot.wav"; }
    std::string reload_sound() final { return "shotgunreload.wav"; }

    int max_primary() final { return 6; }
    int max_secondary() final { return 48; }

    float primary_rate() final { return 1.0f; }

    uint8_t block_damage() final { return 48; }

    float reload_time() final { return 0.5f; }
    glm::vec2 recoil() final { return { -0.1, 0.0002f }; }
    float spread() final { return 0.012f; }
};