#include "camera.h"

#include <algorithm>

#include "SDL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"

#include "game_client.h"
#include "common.h"
#include "scene/game.h"


Camera::Camera(ace::scene::GameScene &s, glm::vec3 position, glm::vec3 forward, glm::vec3 world_up):
    position(position),
    forward(forward),
    right(normalize(cross(world_up, forward))),
    up(cross(forward, right)),
    world_up(world_up),
    scene(s),
    planes{} {

    this->normal_sensitivity = this->scene.client.config.json["controls"].value("mouse_sensitivity", 0.3f);
    this->zoom_sensitivity = this->scene.client.config.json["controls"].value(
        "ads_sensitivity", this->normal_sensitivity);

    this->sensitivity = this->normal_sensitivity;

    auto ang(ace::dir2ang(this->forward));
    this->yaw = ang.x;
    this->pitch = ang.y;

    this->update_view();
}

void Camera::update(double dt) {
    this->mouse(dt);
    this->keyboard(dt);
    this->update_view();
}

void Camera::update_view() {
    glm::mat4 old_pv(pv);

    _view = lookAt(position, position + forward, up);
    pv = _projection * _view;
    forward = normalize(ace::ang2dir(yaw, pitch));
    right = normalize(cross(world_up, forward));
    up = cross(forward, right);

    if (!this->scene.thirdperson && this->scene.ply)
        this->scene.ply->set_orientation(ace::draw2vox(this->forward));
    this->scene.client.sound.set_listener(this->position, this->forward, this->up);

    this->scene.uniforms->view = this->view();
    this->scene.uniforms->proj = this->projection();
    this->scene.uniforms->pv = this->matrix();
    this->scene.uniforms->cam_pos = this->position;
    this->scene.uniforms->cam_forward = this->forward;
    this->scene.uniforms->cam_right = this->right;
    this->scene.uniforms->cam_up = this->up;
    

    if (old_pv == pv) return;

    const auto r3(row(pv, 3));
    this->planes[FRUSTUM_LEFT] = normalize(r3 + row(pv, 0));
    this->planes[FRUSTUM_RIGHT] = normalize(r3 - row(pv, 0));
    this->planes[FRUSTUM_TOP] = normalize(r3 - row(pv, 1));
    this->planes[FRUSTUM_BOTTOM] = normalize(r3 + row(pv, 1));
    this->planes[FRUSTUM_NEAR] = normalize(r3 + row(pv, 2));
    this->planes[FRUSTUM_FAR] = normalize(r3 - row(pv, 2));
}

void Camera::set_projection(float fov, float w, float h, float nearc, float farc) {
    this->nearc = nearc;
    this->farc = farc;
    // if(this->scene.client.keyboard.keys[SDL_SCANCODE_F11]) {
    //     this->_projection = glm::ortho<float>(-20, 20, -20, 20, nearc, farc);
    // } else {
        this->_projection = glm::perspective(glm::radians(fov), w / h, nearc, farc);
    // }
    
}

glm::vec2 Camera::local_to_screen(glm::vec3 position) const {
    glm::vec4 clip = projection() * glm::vec4(position, 1.0f);
    if (clip.w == 0) clip.w = 0.001f;
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    return glm::round(glm::vec2(ndc.x + 1, 1 - ndc.y) / 2.0f * this->scene.client.size());
}

void Camera::mouse(double dt) {
    if (!this->scene.client.exclusive_mouse()) return;
    this->yaw += this->scene.client.mouse.dx * this->sensitivity;
    this->pitch = std::max(-89.9f, std::min(this->pitch - this->scene.client.mouse.dy * this->sensitivity, 89.9f));
}

void Camera::keyboard(double dt) {
    if (!this->scene.thirdperson && this->scene.ply) return;

    const Uint8 *keyboard = this->scene.client.keyboard.keys;
    float speed = this->speed * dt;
    if (keyboard[this->scene.client.config.get_key("sprint", SDL_SCANCODE_LSHIFT)])
        speed *= 3.0f;
    if (keyboard[this->scene.client.config.get_key("forward", SDL_SCANCODE_W)])
        this->position += speed * this->forward;
    if (keyboard[this->scene.client.config.get_key("back", SDL_SCANCODE_S)])
        this->position -= speed * this->forward;
    if (keyboard[this->scene.client.config.get_key("right", SDL_SCANCODE_D)])
        this->position += normalize(cross(this->forward, this->up)) * speed;
    if (keyboard[this->scene.client.config.get_key("left", SDL_SCANCODE_A)])
        this->position -= normalize(cross(this->forward, this->up)) * speed;
    if (keyboard[this->scene.client.config.get_key("jump", SDL_SCANCODE_SPACE)] && !this->scene.thirdperson)
        this->position += speed * this->up;
    if (keyboard[this->scene.client.config.get_key("crouch", SDL_SCANCODE_LCTRL)] && !this->scene.thirdperson)
        this->position -= speed * this->up;
}

bool Camera::box_in_frustum(float x0, float y0, float z0, float x1, float y1, float z1) {
    frustum_vec points[8] {
        { x0, y0, z0, 0.f },
        { x0, y0, z1, 0.f },
        { x0, y1, z0, 0.f },
        { x0, y1, z1, 0.f },
        { x1, y0, z0, 0.f },
        { x1, y0, z1, 0.f },
        { x1, y1, z0, 0.f },
        { x1, y1, z1, 0.f }
    };

    for(const auto &plane : this->planes) {
        bool in = false;
        for (int i = 0; i < 8 && !in; i++) {
            in |= dot(plane, points[i]) >= -plane.w;
        }
        if (!in) return false;
    }
    return true;
}