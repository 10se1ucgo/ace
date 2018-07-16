#pragma once

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

    namespace colors {
        inline glm::vec3 red() { return { 1.f, 0.f, 0.f }; }
        inline glm::vec3 orange() { return { 1.f, 0.5f, 0.f }; }
        inline glm::vec3 yellow() { return { 1.f, 1.f, 0.f }; }
        inline glm::vec3 green() { return { 0.f, 1.f, 0.f }; }
        inline glm::vec3 blue() { return { 0.f, 0.f, 1.f }; }
        inline glm::vec3 purple() { return { 0.5f, 0.f, 1.f }; }
    };
}}