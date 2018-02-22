#include "scene/game.h"
#include "world/tracer.h"


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
        if (this->time_alive >= Tracer::LIFESPAN || scene.map.clipbox(lround(this->position.x), lround(this->position.y), lround(this->position.z))) {
            return true;
        }

        this->position += this->orientation * float(Tracer::SPEED * dt);
        this->mesh.position = vox2draw(this->position); // { this->position.x, -this->position.z + 1, this->position.y };
        this->mesh.position.y += 1;
        return false;
    }

    void Tracer::draw() {
        scene.shaders.model.bind();
        scene.shaders.model.uniform("replacement_color", glm::vec3{ 0.f });
        this->mesh.draw(scene.cam.matrix(), scene.shaders.model);
    }
}}