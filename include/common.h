#pragma once 
#include <random>
#include <chrono>
#include <cmath>

#include "fmt/format.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/type_trait.hpp"
#include "glm/gtx/string_cast.hpp"

namespace ace {
    // Simple, generic wrappers around standard C++ <random>
    namespace random {
        inline std::default_random_engine &engine() {
            static std::default_random_engine eng(std::random_device{}());
            return eng;
        }

        // Generate random integer number between [min, max]
        template<typename T>
        std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value, T> random(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
            return std::uniform_int_distribution<T>(min, max)(engine());
        }

        // Generate random real number between [min, max)
        // Rougly equivalent to `ace::random::random() * (max - min) + min`
        template<typename T>
        std::enable_if_t<std::is_floating_point<T>::value, T> random(T min, T max) {
            return std::uniform_real_distribution<T>(min, max)(engine());
        }

        // Generate random real number between [0, 1)
        template<typename T = double, size_t bits = std::numeric_limits<T>::digits>
        std::enable_if_t<std::is_floating_point<T>::value, T> random() {
            return std::generate_canonical<T, bits>(engine());
        }

        template<typename T>
        std::enable_if_t<std::is_same<T, bool>::value, T> random(double p = 0.5) {
            return std::bernoulli_distribution(p)(engine());
        }

        template<typename T>
        std::enable_if_t<std::is_enum<T>::value, T> choice_range(T first, T last) {
            using underlying = std::underlying_type_t<T>;
            using type = std::conditional_t<std::is_same<signed char, std::make_signed_t<underlying>>::value, int, underlying>;
            // uniform_int_distribution prohibits char types, for whatever reason.
            return T(random::random(type(first), type(last)));
        }
    }

    inline glm::vec3 rand_normalized() {
        float z = random::random(-1.f, 1.f);
        float v = random::random(0.f, glm::two_pi<float>());
        float r = glm::sqrt(1.f - z * z);
        float x = r * glm::cos(v);
        float y = r * glm::sin(v);
        return { x, y, z };
    }

    // euler to direction
    inline glm::vec3 ang2dir(float yaw, float pitch) {
        yaw = glm::radians(yaw);
        pitch = glm::radians(pitch);
        return { glm::cos(pitch) * glm::cos(yaw),
                 glm::sin(pitch),
                 glm::cos(pitch) * glm::sin(yaw) };
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
        glm::vec3 rot(glm::radians(m_rotation));
        model = glm::translate(model, m_position);
        model = glm::rotate(model, rot.z, { 0, 0, 1 });
        model = glm::rotate(model, rot.y, { 0, 1, 0 });
        model = glm::rotate(model, rot.x, { 1, 0, 0 });
        return  glm::scale(model, m_scale);
    }

    inline float wave(float x, float a, float b) {
        return (glm::cos(x * glm::pi<float>()) * (a - b) + (a + b)) / 2.f;
    }

    constexpr uint32_t pack_bytes(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        return uint32_t(a) << 24 | uint32_t(r) << 16 | uint32_t(g) << 8 | uint32_t(b) << 0;
    }

    constexpr void unpack_bytes(uint32_t color, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b) {
        *a = color >> 24 & 0xFF;
        *r = color >> 16 & 0xFF;
        *g = color >> 8 & 0xFF;
        *b = color >> 0 & 0xFF;
    }

    inline glm::u8vec4 unpack_argb(uint32_t color) {
        glm::u8vec4 ret;
        unpack_bytes(color, &ret.a, &ret.r, &ret.g, &ret.b);
        return ret;
    }

    inline glm::u8vec4 unpack_rgba(uint32_t color) {
        glm::u8vec4 ret;
        unpack_bytes(color, &ret.r, &ret.g, &ret.b, &ret.a);
        return ret;
    }
}

namespace fmt {
    // got tired of having to do glm::to_string zzz
    template <typename ArgFormatter, template <typename, glm::precision> class matType, typename T, glm::precision P>
    void format_arg(fmt::BasicFormatter<char, ArgFormatter> &f, const char *&format_str, const matType<T, P> &x) {
        f.writer() << glm::to_string(x);
    }
}


#define ACE_NO_COPY(T) \
    T(const T &other) = delete; \
    void operator=(const T &other) = delete; \

#define ACE_NO_COPY_MOVE(T) \
    ACE_NO_COPY(T) \
    T(T&&) = delete; \
    void operator=(T&&) = delete;
