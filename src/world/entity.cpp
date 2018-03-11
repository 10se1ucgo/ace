#include "world/entity.h"

#include "scene/game.h"

namespace ace { namespace world {

    Entity::Entity(scene::GameScene& scene, uint8_t id, glm::vec3 position, net::TEAM team, uint8_t carrier, const std::string& mesh, float scale):
        WorldObject(scene),
        id(id),
        mesh(scene.models.get(mesh), scale) {
        this->set_position(position);
        this->set_carrier(carrier);
        this->set_team(team);
    }

    Entity::~Entity() {
        this->set_carrier(-1);
    }

    bool Entity::update(double dt) {
        return false;
    }

    void Entity::draw() {
        if (this->carrier < 32 && this->scene.get_ply(this->carrier, false) != nullptr) return;

        scene.shaders.model.bind();
        scene.shaders.model.uniform("replacement_color", this->scene.teams[this->team].float_color);
        this->mesh.draw(scene.cam.matrix(), scene.shaders.model);
    }

    void Entity::set_position(glm::vec3 pos) {
        this->position = pos;
        this->mesh.position = vox2draw(position);
    }

    void Entity::set_team(net::TEAM team) {
        this->team = team;
    }

    void Entity::set_carrier(uint8_t carrier) {
        this->carrier = carrier;
    }
}}