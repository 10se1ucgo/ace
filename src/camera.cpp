#include "camera.h"

#include "SDL.h"
#include "fmt/format.h"
#include <algorithm>

#include "common.h"
#include "scene/game.h"

#include "glm/gtc/matrix_access.hpp"

Camera::Camera(ace::scene::GameScene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up):
    pitch(0), yaw(0), position(position),
    forward(forward), right(normalize(cross(world_up, forward))), up(cross(forward, right)),
    world_up(world_up),
    scene(s) {
}

void Camera::update(double dt) {
    this->mouse(dt);
    this->keyboard(dt);
    this->update();
}

void Camera::update() {
    glm::mat4 old_pv(pv);

    view = lookAt(position, position + forward, up);
    pv = projection * view;
    forward = normalize(ang2dir(yaw, pitch));
    right = normalize(cross(world_up, forward));
    up = cross(forward, right);

    if (old_pv == pv) return;

    planes[FRUSTUM_LEFT] = normalize(row(pv, 3) + row(pv, 0));
    planes[FRUSTUM_RIGHT] = normalize(row(pv, 3) - row(pv, 0));
    planes[FRUSTUM_TOP] = normalize(row(pv, 3) - row(pv, 1));
    planes[FRUSTUM_BOTTOM] = normalize(row(pv, 3) + row(pv, 1));
    planes[FRUSTUM_NEAR] = normalize(row(pv, 3) + row(pv, 2));
    planes[FRUSTUM_FAR] = normalize(row(pv, 3) - row(pv, 2));
}

void Camera::mouse(double dt) {
    yaw += scene.client.mouse.dx * sensitivity;
    pitch = std::max(-89.9f, std::min(pitch - scene.client.mouse.dy * sensitivity, 89.9f));
}

void Camera::keyboard(double dt) {
    if (!scene.thirdperson) return;

    const Uint8 *keyboard = scene.client.keyboard.keys;
    float speed = this->speed * dt;
    if (keyboard[SDL_SCANCODE_LSHIFT])
        speed *= 3.0f;
    if (keyboard[SDL_SCANCODE_W])
        this->position += speed * this->forward;
    if (keyboard[SDL_SCANCODE_S])
        this->position -= speed * this->forward;
    if (keyboard[SDL_SCANCODE_D])
        this->position += normalize(cross(this->forward, this->up)) * speed;
    if (keyboard[SDL_SCANCODE_A])
        this->position -= normalize(cross(this->forward, this->up)) * speed;
    if (keyboard[SDL_SCANCODE_SPACE] && !scene.thirdperson)
        this->position += speed * this->up;
    if (keyboard[SDL_SCANCODE_LCTRL] && !scene.thirdperson)
        this->position -= speed * this->up;
}

bool Camera::box_in_frustum(int x0, int y0, int z0, int x1, int y1, int z1) {
    glm::vec3 points[8];
    points[0] = { x0, y0, z0 };
    points[1] = { x0, y0, z1 };
    points[2] = { x0, y1, z0 };
    points[3] = { x0, y1, z1 };
    points[4] = { x1, y0, z0 };
    points[5] = { x1, y0, z1 };
    points[6] = { x1, y1, z0 };
    points[7] = { x1, y1, z1 };

    for(const auto &plane : planes) {
        bool in = false;
        auto p = glm::vec3(plane);
        for (int i = 0; i < 8 && !in; i++) {
            in |= dot(p, points[i]) >= -plane.w;
        }
        if (!in) return false;
    }
    return true;
}
