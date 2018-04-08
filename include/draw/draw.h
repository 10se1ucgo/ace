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

    namespace Layer {
        enum Layer : int {
            BACKGROUND = -100,
            FRAME,
            FRAME_CONTENT,
            FOREGROUND = 0
        };
    }
}}