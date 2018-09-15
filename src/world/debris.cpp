#include "world/debris.h"

#include "scene/game.h"

namespace ace { namespace world {
    Debris::Debris(glm::vec3 position, glm::vec3 velocity, glm::vec3 color) :
        p(position),
        v(velocity),
        color(color),
        life(MAX_LIFE) {
    }

    bool Debris::update(double dt, World &world) {
        glm::vec3 fpos = this->p;

        this->life -= dt;
        if (this->life <= 0) return false;

        this->v.z += dt;
        this->p += this->v * (float(dt) * 32);

        glm::ivec3 lp(glm::floor(this->p));
        if (world.clipworld(lp.x, lp.y, lp.z)) {
            glm::ivec3 lp2(floor(fpos.x), floor(fpos.y), floor(fpos.z));
            if (lp.z != lp2.z && ((lp.x == lp2.x && lp.y == lp2.y) || !world.clipworld(lp.x, lp.y, lp2.z)))
                this->v.z = -this->v.z;
            else if (lp.x != lp2.x && ((lp.y == lp2.y && lp.z == lp2.z) || !world.clipworld(lp2.x, lp.y, lp.z)))
                this->v.x = -this->v.x;
            else if (lp.y != lp2.y && ((lp.x == lp2.x && lp.z == lp2.z) || !world.clipworld(lp.x, lp2.y, lp.z)))
                this->v.y = -this->v.y;
            this->v *= 0.5f;
            this->p = fpos;
            return true;
        }
        return false;
    }

    DebrisGroup::DebrisGroup(scene::GameScene& scene): WorldObject(scene) {
    }

    void DebrisGroup::add(glm::vec3 position, glm::u8vec3 color, float velocity_mod, int number) {
        this->debris.reserve(debris.size() + number);
        for (int i = 0; i < number; i++) {
            this->debris.emplace_back(position, rand_normalized() * velocity_mod, glm::vec3(jit_color(color)) / 255.f);
        }
    }

    bool DebrisGroup::update(double dt) {
        for(auto &d : this->debris) {
            d.update(dt, this->scene.world);
        }

        this->debris.erase(
            std::remove_if(this->debris.begin(), this->debris.end(), [](auto &debris) { return debris.life <= 0; }),
            this->debris.end()
        );

        return false;
    }

    void DebrisGroup::draw() {
        for(const auto &d : this->debris) {
            auto pos = vox2draw(d.p);
            if (this->scene.cam.point_in_frustum(pos))
                this->scene.billboards.draw({ pos, d.color, d.life * .1f });
        }
    }
}}
