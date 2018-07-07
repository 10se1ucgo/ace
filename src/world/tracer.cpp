#include "world/tracer.h"
#include "scene/game.h"


namespace ace { namespace world {
    Tracer::Tracer(scene::GameScene& scene, const std::string &mesh, glm::vec3 position, glm::vec3 orientation):
        WorldObject(scene),
        mesh(scene.models.get(mesh), 0.1f),
        position(position),
        orientation(orientation) {

        auto angles = dir2ang(vox2draw(orientation));
        this->mesh.rotation = glm::vec3{ -angles.y, -angles.x + 90, 0 };
    }

    bool Tracer::update(double dt) {
        this->time_alive += dt;
        if (this->time_alive >= Tracer::LIFESPAN || scene.map.clipbox(this->position.x, this->position.y, this->position.z)) {
            return true;
        }

        this->position += this->orientation * float(Tracer::SPEED * dt);
        return false;
    }

    void Tracer::draw() {
        this->scene.shaders.model.bind();
        this->scene.shaders.model.uniform("replacement_color", glm::vec3{ 0.f });
        this->mesh.position = vox2draw(this->position);
        this->mesh.draw(this->scene.shaders.model);
    }
}}