#pragma once
#include "glm/glm.hpp"

#include "gl/gl_util.h"
#include "gl/shader.h"

namespace ace { namespace draw {
#pragma pack(push, 1)
    struct Billboard {
        glm::vec3 position;
        glm::vec3 color;
        float size;
    };
#pragma pack(pop)

    struct BillboardManager {
        BillboardManager();

        void draw(Billboard bb);
        void draw(gl::ShaderProgram &s);

        gl::experimental::vao vao;
        gl::experimental::streaming_vbo<Billboard> vbo;
    };
}}