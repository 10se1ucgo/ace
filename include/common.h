#pragma once
#include <random>
#include <chrono>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/type_trait.hpp"

namespace ace {
    namespace random {
        inline std::default_random_engine &engine() {
            static std::default_random_engine eng(std::chrono::system_clock::now().time_since_epoch().count());
            return eng;
        }

        template<typename T>
        std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value, T> random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
            return std::uniform_int_distribution<T>(min, max)(engine());
        }

        template<typename T = double>
        std::enable_if_t<std::is_floating_point<T>::value, T> random(T min = T{0}, T max = T{1}) {
            return std::uniform_real_distribution<T>(min, max)(engine());
        }

        template<typename T>
        std::enable_if_t<std::is_same<T, bool>::value, T> random(double p = 0.5) {
            return std::bernoulli_distribution(p)(engine());
        }
    }

    inline glm::vec3 rand_normalized() {
        float z = ace::random::random(-1.f, 1.f);
        float v = ace::random::random(0.f, glm::two_pi<float>());
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
}


#define ACE_NO_COPY(T) \
    T(const T &other) = delete; \
    void operator=(const T &other) = delete; \

#define ACE_NO_COPY_MOVE(T) \
    ACE_NO_COPY(T) \
    T(T&&) = delete; \
    void operator=(T&&) = delete; \
