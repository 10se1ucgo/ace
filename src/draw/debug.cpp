#include "draw/debug.h"

namespace ace { namespace draw {
    DebugDraw::DebugDraw() {
        this->vao.attrib_pointer("3f,4f", this->vbo.handle);
    }

    void DebugDraw::draw_cube(glm::vec3 center, glm::vec3 size, glm::vec3 color) {
        size /= 2.f;

        this->draw_quad(glm::vec3(-size.x, -size.y, size.z) + center,
                        glm::vec3(size.x, -size.y, size.z) + center,
                        glm::vec3(size.x, size.y, size.z) + center,
                        glm::vec3(-size.x, size.y, size.z) + center, color);
        this->draw_quad(glm::vec3(-size.x, -size.y, -size.z) + center,
                        glm::vec3(-size.x, size.y, -size.z) + center,
                        glm::vec3(size.x, size.y, -size.z) + center,
                        glm::vec3(size.x, -size.y, -size.z) + center, color);
        this->draw_quad(glm::vec3(-size.x, -size.y, size.z) + center,
                        glm::vec3(-size.x, size.y, size.z) + center,
                        glm::vec3(-size.x, size.y, -size.z) + center,
                        glm::vec3(-size.x, -size.y, -size.z) + center, color);
        this->draw_quad(glm::vec3(size.x, -size.y, -size.z) + center,
                        glm::vec3(size.x, size.y, -size.z) + center,
                        glm::vec3(size.x, size.y, size.z) + center,
                        glm::vec3(size.x, -size.y, size.z) + center, color);
        this->draw_quad(glm::vec3(-size.x, size.y, size.z) + center,
                        glm::vec3(size.x, size.y, size.z) + center,
                        glm::vec3(size.x, size.y, -size.z) + center,
                        glm::vec3(-size.x, size.y, -size.z) + center, color);
        this->draw_quad(glm::vec3(-size.x, -size.y, size.z) + center,
                        glm::vec3(-size.x, -size.y, -size.z) + center,
                        glm::vec3(size.x, -size.y, -size.z) + center,
                        glm::vec3(size.x, -size.y, size.z) + center, color);
    }

    void DebugDraw::flush(const glm::mat4 &pv, gl::ShaderProgram &s) {
        if (this->vbo->empty()) return;

        s.uniform("mvp", pv);
        this->vbo.upload();
        this->vao.draw(GL_LINES, this->vbo.draw_count, this->vbo.draw_offset);
    }
}}
