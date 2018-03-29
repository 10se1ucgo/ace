#include "camera.h"

#include <algorithm>

#include "SDL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"

#include "common.h"
#include "scene/game.h"


Camera::Camera(ace::scene::GameScene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up):
    pitch(0), yaw(0), position(position),
    forward(forward), right(normalize(cross(world_up, forward))), up(cross(forward, right)),
    world_up(world_up),
    scene(s) {
}

void Camera::update(double dt) {
    this->mouse(dt);
    this->keyboard(dt);
    this->update_view();
}

void Camera::update_view() {
    glm::mat4 old_pv(pv);

    view = lookAt(position, position + forward, up);
    pv = projection * view;
    forward = normalize(ace::ang2dir(yaw, pitch));
    right = normalize(cross(world_up, forward));
    up = cross(forward, right);

    if (!this->scene.thirdperson && this->scene.ply)
        this->scene.ply->set_orientation(this->forward.x, this->forward.z, -this->forward.y);
    this->scene.client.sound.set_listener(this->position, this->forward, this->up);

    if (old_pv == pv) return;

    const auto r3(row(pv, 3));
    planes[FRUSTUM_LEFT] = normalize(r3 + row(pv, 0));
    planes[FRUSTUM_RIGHT] = normalize(r3 - row(pv, 0));
    planes[FRUSTUM_TOP] = normalize(r3 - row(pv, 1));
    planes[FRUSTUM_BOTTOM] = normalize(r3 + row(pv, 1));
    planes[FRUSTUM_NEAR] = normalize(r3 + row(pv, 2));
    planes[FRUSTUM_FAR] = normalize(r3 - row(pv, 2));
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
    glm::vec3 points[8] {
        { x0, y0, z0 },
        { x0, y0, z1 },
        { x0, y1, z0 },
        { x0, y1, z1 },
        { x1, y0, z0 },
        { x1, y0, z1 },
        { x1, y1, z0 },
        { x1, y1, z1 }
    };

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
