#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scene/scene.h"

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
    float pitch, yaw, speed = 5.f, sensitivity = 0.3f;
    glm::vec3 position, forward, right, up, world_up;
    ace::scene::GameScene &scene;

    Camera(ace::scene::GameScene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up={ 0, 1, 0 });

    const glm::mat4x4 &matrix() const {
        return this->pv;
    }

    

    void update(double dt);
    void update_view();

    void set_projection(float fov, float w, float h, float nearc=0.1f, float farc=256.f) {
        this->nearc = nearc;
        this->farc = farc;
        this->projection = glm::perspective(glm::radians(fov), w / h, nearc, farc);
    }
// private:
    glm::mat4 projection, view, pv;
    float nearc{ 0.1f }, farc{ 256.f };
    glm::vec4 planes[FRUSTUM_NUM_PLANES];

    void mouse(double dt);
    void keyboard(double dt);

    bool box_in_frustum(int x0, int y0, int z0, int x1, int y1, int z1);
};

