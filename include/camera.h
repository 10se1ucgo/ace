#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scene/scene.h"
#include <array>
#include "gl/gl_util.h"
#include "gl/shader.h"

namespace detail {
    enum {
        FRUSTUM_LEFT,
        FRUSTUM_RIGHT,
        FRUSTUM_TOP,
        FRUSTUM_BOTTOM,
        FRUSTUM_NEAR,
        FRUSTUM_FAR,
        FRUSTUM_NUM_PLANES
    };

    struct plane {
        plane() = default;
        explicit plane(const glm::vec4 &p) : normal(p), constant(p.w) {
        }

        glm::vec3 normal;
        float constant;
    };
}

class Camera {
public:
    float yaw{ 0.f }, pitch{ 0.f }, speed{ 5.f }, sensitivity, normal_sensitivity, zoom_sensitivity;
    bool thirdperson{ false };
    glm::vec3 position, forward, right, up, world_up;
    ace::scene::Scene &scene;

    Camera(ace::scene::Scene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up={ 0, 1, 0 });

    const glm::mat4x4 &projection() const {
        return this->_projection;
    }

    const glm::mat4x4 &view() const {
        return this->_view;
    }

    const glm::mat4x4 &matrix() const {
        return this->pv;
    }

    void update(double dt, ace::gl::experimental::ubo<ace::scene::Uniforms3D> &uniforms);
    void update_view(ace::gl::experimental::ubo<ace::scene::Uniforms3D> &uniforms);

    void set_projection(float fov, float w, float h, float nearc = 0.1f, float farc = 256.f);

    bool box_in_frustum(float x0, float y0, float z0, float x1, float y1, float z1);
    bool box_in_frustum(glm::vec3 p0, glm::vec3 p1) {
        return this->box_in_frustum(p0.x, p0.y, p0.z, p1.x, p1.y, p1.z);
    }

    bool point_in_frustum(glm::vec3 point) {
        for (const auto &plane : this->planes) {
            if (dot(plane.normal, point) < -plane.constant) return false;
        }
        return true;
    }

    glm::vec2 local_to_screen(glm::vec3 position) const;
private:
    glm::mat4 _projection, _view, pv;
    float nearc{ 0.1f }, farc{ 256.f };

    detail::plane planes[detail::FRUSTUM_NUM_PLANES];

    void mouse(double dt);
    void keyboard(double dt);
};

