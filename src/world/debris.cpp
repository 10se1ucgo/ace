#include "world/debris.h"

#include "scene/game.h"

namespace ace { namespace world {
    Debris::Debris(DebrisGroup& group, glm::vec3 position, glm::vec3 velocity, glm::u8vec3 color) :
        group(group),
        p(position),
        v(velocity),
        color(color) {
        this->color /= 255.f;
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

    DebrisGroup::DebrisGroup(scene::GameScene& scene, glm::vec3 position, glm::u8vec3 color, float vel_mod, int num):
        WorldObject(scene),
        life(MAX_LIFE) {
        debris.reserve(num);
        for (int i = 0; i < num; i++) {
            debris.emplace_back(*this, position, rand_normalized() * vel_mod, jit_color(color));
        }
    }

    bool DebrisGroup::update(double dt) {
        this->life -= dt;
        if (this->life <= 0) return true;

        for(auto &d : this->debris) {
            d.update(dt);
        }
        return false;
    }

    void DebrisGroup::draw() {
        float size = this->life * .1f;
        for(const auto &d : this->debris) {
            auto pos = vox2draw(d.p);
            if (this->scene.cam.point_in_frustrum(pos))
                this->scene.billboards.draw({ pos, d.color, size });
        }
    }
}}
