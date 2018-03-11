#pragma once
#include "glm/glm.hpp"

#include "gl/gl_obj.h"
#include "gl/shader.h"

namespace ace { namespace draw {
    struct Billboard {
        glm::vec3 position;
        glm::vec3 color;
        float size;
    };

    struct BillboardManager {
        BillboardManager();

        void draw(Billboard bb);
        void draw(const glm::mat4 &pv, ShaderProgram &s);

        gl::vao vao;
        gl::vbo vbo;
        glm::vec3 position;
        std::vector<Billboard> billboards;
    };
}}