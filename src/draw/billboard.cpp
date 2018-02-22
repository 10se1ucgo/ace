#include "draw/billboard.h"
#include "glm/gtc/matrix_transform.hpp"
#include "fmt/printf.h"
#include <glm/gtx/string_cast.hpp>

namespace ace {
    namespace draw {
        BillboardManager::BillboardManager() {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
//            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Billboard), reinterpret_cast<void*>(offsetof(Billboard, position)));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Billboard), reinterpret_cast<void*>(offsetof(Billboard, color)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Billboard), reinterpret_cast<void*>(offsetof(Billboard, size)));
            glEnableVertexAttribArray(2);
        }

        void BillboardManager::draw(const Billboard &bb) {
            this->billboards.push_back(bb);
        }

        void BillboardManager::draw(const glm::mat4 &pv, ShaderProgram &s) {
            s.uniform("pv", pv);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glInvalidateBufferData(GL_ARRAY_BUFFER);
            glBufferData(GL_ARRAY_BUFFER, this->billboards.size() * sizeof(Billboard), this->billboards.data(), GL_STREAM_DRAW);

            glDrawArrays(GL_POINTS, 0, this->billboards.size());

            this->billboards.clear();
        }
    }
}
