#pragma once
#include <random>
#include <chrono>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

extern std::default_random_engine eng;

inline glm::vec3 rand_normalized() {
    const std::uniform_real_distribution<float> dis1(-1.f, 1.f);
    const std::uniform_real_distribution<float> dis2(0.f, glm::two_pi<float>());
    // temp static variables because LAZY
    float z = dis1(eng);
    float v = dis2(eng);
    float r = glm::sqrt(1.f - z * z);
    float x = r * glm::cos(v);
    float y = r * glm::sin(v);
    return { x, y, z };
}

// euler to direction
inline glm::vec3 ang2dir(float yaw, float pitch) {
    return { glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw)),
             glm::sin(glm::radians(pitch)),
             glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw)) };
}

inline glm::vec2 dir2ang(const glm::vec3 &dir) {
    return { glm::degrees(glm::atan(dir.z, dir.x)), glm::degrees(asin(dir.y)) };
}

inline glm::vec3 vox2draw(glm::vec3 vox) {
    return { vox.x, -vox.z, vox.y };
}

inline glm::vec3 draw2vox(glm::vec3 draw) {
    return { draw.x, draw.z, -draw.y };
}

inline glm::mat4 model_matrix(glm::vec3 m_position, glm::vec3 m_rotation, glm::vec3 m_scale, glm::mat4 model = glm::mat4(1.0f)) {
    glm::vec3 rot(radians(m_rotation));
    model = translate(model, m_position);
    model = rotate(model, rot.z, { 0, 0, 1 });
    model = rotate(model, rot.y, { 0, 1, 0 });
    model = rotate(model, rot.x, { 1, 0, 0 });
    return  scale(model, m_scale);
}

// todo not rely on this lol
#define ACE_NO_COPY_MOVE(T) \
    T(const T &other) = delete; \
    void operator=(const T &other) = delete; \
    T(T&&) = delete; \
    void operator=(T&&) = delete; \
