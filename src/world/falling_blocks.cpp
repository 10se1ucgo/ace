#include "world/falling_blocks.h"

#include "scene/game.h"

namespace ace { namespace world {
    FallingBlocks::FallingBlocks(scene::GameScene& scene, const std::vector<VXLBlock>& blocks):
        WorldObject(scene),
        mesh(blocks),
        position(mesh.centroid),
        velocity(0),
        direction(rand_normalized()) {
        this->scene.client.sound.play("debris.wav", vox2draw(position));
    }

    bool FallingBlocks::update(double dt) {
        alpha -= 1/5.0 * dt;

        glm::vec3 fpos = this->position;

        this->velocity.z += dt;
        this->position += this->velocity * float(32 * dt);


        if (this->scene.map.clipworld(this->position.x, this->position.y, this->position.z)) {
            this->position = fpos;
            this->velocity *= -0.6;
            this->direction = rand_normalized();
            alpha -= 1/5.f;
            this->scene.client.sound.play("bounce.wav", vox2draw(position));
        }

        if (alpha <= 0.0) {
            return true;
        }

        this->mesh.rotation += this->direction * float(80 * dt);
        this->mesh.position = ace::vox2draw(this->position);
        return false;
    }

    void FallingBlocks::draw() {
        gl::ShaderProgram &s = scene.shaders.map;
        s.bind();
        s.uniform("alpha", alpha);

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        this->mesh.draw(scene.cam.matrix(), s);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}}