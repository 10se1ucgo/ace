#pragma once
#include "glm/glm.hpp"

#include "gl/gl_util.h"
#include "gl/shader.h"

namespace ace { namespace draw {
#pragma pack(push, 1)
    struct LineVertex {
        glm::vec3 position;
        glm::vec4 color;
        // todo: line thickness float size;
    };
#pragma pack(pop)

    struct DebugDraw {
        DebugDraw();

        void draw_line(glm::vec3 p1, glm::vec3 p2, glm::vec3 color) {
            this->vbo->push_back({ p1, glm::vec4(color, 1.0f) });
            this->vbo->push_back({ p2, glm::vec4(color, 1.0f) });
        }

        void draw_ray(glm::vec3 start, glm::vec3 direction, glm::vec3 color) {
            this->draw_line(start, start + direction, color);
        }

        void draw_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color) {
            this->draw_line(a, b, color);
            this->draw_line(b, c, color);
            this->draw_line(c, a, color);
        }

        void draw_quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 color) {
            this->draw_line(a, b, color);
            this->draw_line(b, c, color);
            this->draw_line(c, d, color);
            this->draw_line(d, a, color);
        }

        void draw_cube(glm::vec3 center, glm::vec3 size, glm::vec3 color);

        void flush(const glm::mat4 &pv, gl::ShaderProgram &s);

        gl::experimental::vao vao;
        gl::experimental::streaming_vbo<LineVertex> vbo;
    };
}}