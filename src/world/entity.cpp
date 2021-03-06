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
        if (!this->visible())
            return;

        this->scene.shaders.model.bind();
        this->scene.shaders.model.uniform("replacement_color", this->scene.get_team(this->_team).float_color);
        this->mesh.position = vox2draw(_position);
        this->mesh.draw(this->scene.shaders.model);
    }

    bool Entity::visible() const {
        return this->_carrier >= 32 || this->scene.get_ply(this->_carrier, false) == nullptr;
    }

    scene::Team &Entity::team() const {
        return this->scene.get_team(this->_team);
    }

    DrawPlayer *Entity::carrier() const {
        return this->scene.get_ply(this->_carrier, false);
    }

    void Entity::set_position(glm::vec3 pos) {
        this->_position = pos;
    }

    void Entity::set_team(net::TEAM team) {
        this->_team = team;
    }

    void Entity::set_carrier(uint8_t carrier) {
        this->_carrier = carrier;
    }
}}