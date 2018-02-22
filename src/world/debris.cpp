#include "world/debris.h"
#include "scene/game.h"
#include "glm/gtc/matrix_access.hpp"

namespace ace { namespace world {
    Debris::Debris(DebrisGroup& group, glm::vec3 position, glm::vec3 velocity) : group(group), p(position), v(velocity) {
    }

    bool Debris::update(double dt) {
        glm::vec3 fpos = this->p;

        this->v.z += dt;
        this->p += this->v * (float(dt) * 32);

        glm::ivec3 lp(floor(this->p.x), floor(this->p.y), floor(this->p.z));
        if (this->group.scene.map.clipworld(lp.x, lp.y, lp.z)) {
            glm::ivec3 lp2(floor(fpos.x), floor(fpos.y), floor(fpos.z));
            if (lp.z != lp2.z && ((lp.x == lp2.x && lp.y == lp2.y) || !this->group.scene.map.clipworld(lp.x, lp.y, lp2.z)))
                this->v.z = -this->v.z;
            else if (lp.x != lp2.x && ((lp.y == lp2.y && lp.z == lp2.z) || !this->group.scene.map.clipworld(lp2.x, lp.y, lp.z)))
                this->v.x = -this->v.x;
            else if (lp.y != lp2.y && ((lp.x == lp2.x && lp.z == lp2.z) || !this->group.scene.map.clipworld(lp.x, lp2.y, lp.z)))
                this->v.y = -this->v.y;
            this->v *= 0.5f;
            this->p = fpos;
            return true;
        }
        return false;
    }

    DebrisGroup::DebrisGroup(scene::GameScene& scene, glm::vec3 position, glm::vec3 color, float vel_mod, int num):
        WorldObject(scene),
        color(color / 255.f),
        life(MAX_LIFE) {
        debris.reserve(num);
        for (int i = 0; i < num; i++) {
            debris.emplace_back(*this, position, rand_normalized() * vel_mod);
        }
    }

    bool DebrisGroup::update(double dt) {
        life -= dt;
        
        if (life <= 0) return true;

        for(auto &d : debris) {
            d.update(dt);
        }
        return false;
    }

    void DebrisGroup::draw() {
        float size = (life + .5f) * .1f;
        for(const auto &d : debris) {
            glm::vec3 p = vox2draw(d.p);
            p.y += 1;
            this->scene.billboards.draw({ p, color, size });
//            mdl.position = vox2draw(d.p);
//            mdl.position.y += 1;
//            mdl.draw(this->scene.cam.matrix(), this->scene.shaders.billboard);
        }
    }
}}
