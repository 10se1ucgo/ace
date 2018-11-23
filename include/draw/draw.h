#pragma once
#include "common.h"

namespace ace { namespace draw {
    enum class Align {
        TOP_LEFT,
        BOTTOM_LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        CENTER,
        TOP_CENTER,
        BOTTOM_CENTER,
    };

    struct Layer {
        // i want a scoped enum that will implicitly convert to int because im weird that way
        enum : int {
            BACKGROUND = -100,
            FRAME,
            FRAME_CONTENT,
            FOREGROUND = 0
        };
    };

    // i will make a color/color32 struct at some point and remove these i promise..

    namespace color32 {
        inline glm::u8vec3 black() { return { 0, 0, 0 }; }
        inline glm::u8vec3 white() { return { 255, 255, 255 }; }
        inline glm::u8vec3 red() { return { 255, 0, 0 }; }
        inline glm::u8vec3 orange() { return { 255, 127, 0 }; }
        inline glm::u8vec3 yellow() { return { 255, 255, 0 }; }
        inline glm::u8vec3 green() { return { 0, 255, 0 }; }
        inline glm::u8vec3 blue() { return { 0, 0, 255 }; }
        inline glm::u8vec3 purple() { return { 127, 0, 255 }; }
        inline glm::u8vec3 random() { return { random::random(0, 255), random::random(0, 255), random::random(0, 255) }; }
    };

    namespace color {
        inline glm::vec3 black() { return { 0.f, 0.f, 0.f }; }
        inline glm::vec3 white() { return { 1.f, 1.f, 1.f }; }
        inline glm::vec3 red() { return { 1.f, 0.f, 0.f }; }
        inline glm::vec3 orange() { return { 1.f, 0.5f, 0.f }; }
        inline glm::vec3 yellow() { return { 1.f, 1.f, 0.f }; }
        inline glm::vec3 green() { return { 0.f, 1.f, 0.f }; }
        inline glm::vec3 blue() { return { 0.f, 0.f, 1.f }; }
        inline glm::vec3 purple() { return { 0.5f, 0.f, 1.f }; }
        inline glm::vec3 random() { return glm::vec3(color32::random()) / 255.f; }
    };
}}