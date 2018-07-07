#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scene/scene.h"
#include <array>

enum {
    FRUSTUM_LEFT,
    FRUSTUM_RIGHT,
    FRUSTUM_TOP,
    FRUSTUM_BOTTOM,
    FRUSTUM_NEAR,
    FRUSTUM_FAR,
    FRUSTUM_NUM_PLANES
};


class Camera {
public:
    float yaw{ 0.f }, pitch{ 0.f }, speed{ 5.f }, sensitivity, normal_sensitivity, zoom_sensitivity;
    glm::vec3 position, forward, right, up, world_up;
    ace::scene::GameScene &scene;

    Camera(ace::scene::GameScene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up={ 0, 1, 0 });

    const glm::mat4x4 &projection() const {
        return this->_projection;
    }

    const glm::mat4x4 &view() const {
        return this->_view;
    }

    const glm::mat4x4 &matrix() const {
        return this->pv;
    }

    void update(double dt);
    void update_view();

    void set_projection(float fov, float w, float h, float nearc = 0.1f, float farc = 256.f);

    bool box_in_frustum(float x0, float y0, float z0, float x1, float y1, float z1);
    bool box_in_frustum(glm::vec3 p0, glm::vec3 p1) {
        return this->box_in_frustum(p0.x, p0.y, p0.z, p1.x, p1.y, p1.z);
    }

    bool point_in_frustrum(glm::vec3 point) {
        for (const auto &plane : this->planes) {
            if (dot(glm::vec3(plane), point) < -plane.w) return false;
        }
        return true;
    }
private:
    glm::mat4 _projection, _view, pv;
    float nearc{ 0.1f }, farc{ 256.f };

    using frustrum_vec = glm::vec4;

    frustrum_vec planes[FRUSTUM_NUM_PLANES];

    void mouse(double dt);
    void keyboard(double dt);
};

