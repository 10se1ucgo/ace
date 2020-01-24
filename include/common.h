#pragma once 
#include <random>
#include <chrono>
#include <cmath>

#include "fmt/format.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/type_trait.hpp"
#include "glm/gtx/string_cast.hpp"
#include "SDL_filesystem.h"

namespace ace {
    inline const std::string &resource_dir() {
        static struct PathFinder { // hah
            // If im correct PathFinder's constructor should be called on the first call to this
            // this way the path will be initialized (hopefully) after SDL_Init (provided nobody calls this before GameClient's ctor)
            
            // I can't put this in GameClient because most of the resource managers have no attachment to the client or scene
            // but they need to be able to use this path. Also don't want to make it a global variable because it shouldn't be mutable.
            // I also don't want to call SDL_GetBasePath often, because the documentation says it might not be necessarily fast.
            // It's also a useless number of memory allocations.

            // Can't use an if statement because an empty string is valid (SDL_GBP returns nullptr on error)
            // And making a string/bool pair is not CLEVER ENOUGH FOR ME
            PathFinder() {
                char *base_path = SDL_GetBasePath();
                this->path.assign(base_path ? base_path : "./");
                SDL_free(base_path);
            }
            std::string path;
        } finder;
        return finder.path;
    }

    // Currently just adds the resource dir to the resource path, but maybe later I'll need to expand it.
    inline std::string get_resource_path(const std::string &resource) {
        return resource_dir() + resource;
    }

    // Simple, generic wrappers around standard C++ <random>
    namespace random {
        inline std::minstd_rand &engine() {
            static std::minstd_rand eng(std::random_device{}());
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

    // (yaw, pitch) to direction vec3
    inline glm::vec3 ang2dir(float yaw, float pitch) {
        yaw = glm::radians(yaw);
        pitch = glm::radians(pitch);
        return { glm::cos(pitch) * glm::cos(yaw),
                 glm::sin(pitch),
                 glm::cos(pitch) * glm::sin(yaw) };
    }

    // direction vector to (yaw, pitch)
    inline glm::vec2 dir2ang(glm::vec3 dir) {
        return { glm::degrees(glm::atan(dir.z, dir.x)), glm::degrees(asin(dir.y)) };
    }

    // convert VOXLAP position (Z is depth) to OpenGL position (Y is height)
    inline glm::vec3 vox2draw(glm::vec3 vox) {
        return { vox.x, -vox.z, vox.y };
    }

    // convert OpenGL position (Y is height) to VOXLAP position (Z is depth)
    inline glm::vec3 draw2vox(glm::vec3 draw) {
        return { draw.x, draw.z, -draw.y };
    }

    // Generate a model matrix from `model` translated by `m_position`, rotated by `m_rotation` and scaled by `m_scale`. 
    inline glm::mat4 model_matrix(glm::vec3 m_position, glm::vec3 m_rotation, glm::vec3 m_scale = glm::vec3(1.0f), glm::mat4 model = glm::mat4(1.0f)) {
        glm::vec3 rot(glm::radians(m_rotation));
        model = glm::translate(model, m_position);
        model = glm::rotate(model, rot.z, { 0, 0, 1 });
        model = glm::rotate(model, rot.y, { 0, 1, 0 });
        model = glm::rotate(model, rot.x, { 1, 0, 0 });
        return  glm::scale(model, m_scale);
    }

    // cosine wave generator
    inline float coswave(float x, float a, float b) {
        return (glm::cos(x * glm::pi<float>()) * (a - b) + (a + b)) * 0.5f;
    }

    inline float triwave(float x, float a, float b, float linger = 0) {
        return glm::clamp(glm::abs(glm::mod(x, 2.f) - 1.f) * ((a + linger) - (b - linger)) + (b - linger), a, b);
    }

    constexpr uint32_t pack_bytes(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        return uint32_t(a) << 24 | uint32_t(r) << 16 | uint32_t(g) << 8 | uint32_t(b) << 0;
    }

    inline uint32_t pack_argb(glm::u8vec4 color) {
        return pack_bytes(color.a, color.r, color.g, color.b);
    }

    inline uint32_t pack_rgba(glm::u8vec4 color) {
        return pack_bytes(color.r, color.g, color.b, color.a);
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
    template <typename ArgFormatter, class matType>
    void format_arg(fmt::BasicFormatter<char, ArgFormatter> &f, const char *&format_str, matType const& x) {
        f.writer() << glm::to_string(x);
    }
}


#define ACE_NO_COPY(T) \
    T(const T &other) = delete; \
    void operator=(const T &other) = delete;

#define ACE_NO_COPY_MOVE(T) \
    ACE_NO_COPY(T) \
    T(T&&) = delete; \
    void operator=(T&&) = delete;
