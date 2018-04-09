#include "world/grenade.h"

#include "game_client.h"
#include "world/debris.h"
#include "scene/game.h"



namespace ace { namespace world {
    Grenade::Grenade(scene::GameScene& scene, glm::vec3 position, glm::vec3 velocity, float fuse):
        WorldObject(scene),
        mesh(scene.models.get("grenade.kv6"), 0.05f),
        p(position),
        v(velocity),
        fuse(fuse) {
    }

    bool Grenade::update(double dt) {
        fuse -= dt;
        this->move(dt);

        this->mesh.position = vox2draw(this->p);
        this->mesh.rotation += float(-dt * 480) * vox2draw(this->v);
        
        if(fuse <= 0) {
//            this->scene.destroy_point(this->p.x, this->p.y, this->p.z, net::ACTION::GRENADE);

            bool water = this->p.z >= 63;

            for(int i = 1; i <= 3; i++) {
                glm::vec3 color = water ? glm::vec3(51 * i, 51 * (i + 1), 51 * (i + 2)) : glm::vec3(32 * i);
                this->scene.create_object<world::DebrisGroup>(this->p, color, 0.5f * i, 4 * i);
            }
            this->scene.client.sound.play(water ? "waterexplode.wav" : "explode.wav", this->mesh.position);
            return true;
        }
        return false;
    }

    void Grenade::draw() {
        scene.shaders.model.bind();
        scene.shaders.model.uniform("replacement_color", glm::vec3{ 0.f });
        this->mesh.draw(scene.cam.matrix(), scene.shaders.model);
    }

    bool Grenade::move(double dt) {
        glm::vec3 fpos = this->p; //old position

                               //do velocity & gravity (friction is negligible)
        float f = dt * 32;
        this->v.z += dt;
        // yikes, my vector type operator overloading actually has a use!!!
        this->p += this->v * f;

        //make it bounce (accurate)
        glm::ivec3 lp(floor(this->p.x), floor(this->p.y), floor(this->p.z));

        if (this->scene.map.clipworld(lp.x, lp.y, lp.z)) { //hit a wall
            if (fabs(this->v.x) > 0.1f ||
                fabs(this->v.y) > 0.1f ||
                fabs(this->v.z) > 0.1f) {
                this->scene.client.sound.play("grenadebounce.wav", this->mesh.position);
            }

            glm::ivec3 lp2(floor(fpos.x), floor(fpos.y), floor(fpos.z));
            if (lp.z != lp2.z && ((lp.x == lp2.x && lp.y == lp2.y) || !this->scene.map.clipworld(lp.x, lp.y, lp2.z)))
                this->v.z = -this->v.z;
            else if (lp.x != lp2.x && ((lp.y == lp2.y && lp.z == lp2.z) || !this->scene.map.clipworld(lp2.x, lp.y, lp.z)))
                this->v.x = -this->v.x;
            else if (lp.y != lp2.y && ((lp.x == lp2.x && lp.z == lp2.z) || !this->scene.map.clipworld(lp.x, lp2.y, lp.z)))
                this->v.y = -this->v.y;

            this->p = fpos; //set back to old position
            this->v *= 0.36;
            return true;
        }
        return false;
    }
}}
