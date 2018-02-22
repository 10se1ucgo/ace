#pragma once

#include "glad/glad.h"
#include "gl/shader.h"
#include "glm/glm.hpp"
#include "SDL.h"

#include <string>

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
        ~BillboardManager() {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }

        void draw(const Billboard &bb);
        void draw(const glm::mat4 &pv, ShaderProgram &s);

        GLuint vao, vbo;
        glm::vec3 position;
        std::vector<Billboard> billboards;
    };
}}