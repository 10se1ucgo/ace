#pragma once
#include "glad/glad.h"

#include "common.h"

#include <algorithm>
#include "glad/glad.h"

namespace ace { namespace gl {
    typedef void (*TGENERATOR)(GLsizei n, GLuint *handles);
    typedef void (*TDELETER)(GLsizei n, const GLuint *handles);

    template<TGENERATOR TGenerator, TDELETER TDeleter, typename THandle=GLuint>
    struct GLObj {
        THandle handle;

        GLObj() {
            TGenerator(1, &handle);
        }

        ~GLObj() {
            this->release();
        }

        GLObj(GLObj &&that) noexcept : handle(that.handle) {
            that.handle = 0;
        }

        GLObj &operator=(GLObj &&that) noexcept {
            if (this != &that) {
                this->release();
                std::swap(this->handle, that.handle);
            }
            return *this;
        }

        operator THandle() const { return this->handle; }

        ACE_NO_COPY(GLObj)
    private:
        void release() {
            TDeleter(1, &this->handle);
            this->handle = 0;
        }
    };

    namespace detail {
        // beccaus i cant use the glad function ptr directly
        // because lambdas can't be constant expressions until C++17 :(
        inline auto gen_vert_array(GLsizei n, GLuint *handles) { return glGenVertexArrays(n, handles); }
        inline auto del_vert_array(GLsizei n, const GLuint *handles) { return glDeleteVertexArrays(n, handles); }

        inline auto gen_buffer(GLsizei n, GLuint *handles) { return glGenBuffers(n, handles); }
        inline auto del_buffer(GLsizei n, const GLuint *handles) { return glDeleteBuffers(n, handles); }

        inline auto gen_texture(GLsizei n, GLuint *handles) { return glGenTextures(n, handles); }
        inline auto del_texture(GLsizei n, const GLuint *handles) { return glDeleteTextures(n, handles); }
    }

    using vao = GLObj<detail::gen_vert_array, detail::del_vert_array>;
    using vbo = GLObj<detail::gen_buffer, detail::del_buffer>;
    using texture = GLObj<detail::gen_texture, detail::del_texture>;
}}
