#pragma once
#include <glm/glm.hpp>

#include "world/world.h"
#include "draw/map.h"
#include "kv6.h"

namespace ace { namespace world {
    struct Entity : WorldObject {
        Entity(scene::GameScene& scene, uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier,
               const std::string& mesh, float scale=0.1f);
        ~Entity();

        bool update(double dt) override;
        void draw() override;

        void set_position(glm::vec3 pos);
        void set_team(net::TEAM team);
        void set_carrier(uint8_t carrier);

        uint8_t id;
        glm::vec3 position;
        net::TEAM team;
        uint8_t carrier;

        KV6 mesh;
//        draw::SpriteGroup
    };

    struct CommandPost : Entity {
        CommandPost(scene::GameScene& scene, uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier) : Entity(scene, id, position, team, carrier, "cp.kv6", 0.3f) {}
    };

    struct Flag : Entity {
        Flag(scene::GameScene& scene, uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier) : Entity(scene, id, position, team, carrier, "intel.kv6", 0.15f) {}
        
        bool update(double dt) final {
            this->mesh.rotation.y += dt * 10;
            return Entity::update(dt);
        };
    };

//    struct AmmoCrate : Entity {
//        
//    };
//
//    struct HealthCrate : Entity {
//        
//    };
//
    struct Helicopter : Entity {

    };
}}