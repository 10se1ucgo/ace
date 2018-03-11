#pragma once
#include <algorithm>

#include "glad/glad.h"

#include "common.h"

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
        inline auto gen_vertex_arrays(GLsizei n, GLuint *handles) { return glGenVertexArrays(n, handles); }
        inline auto delete_vertex_arrays(GLsizei n, const GLuint *handles) { return glDeleteVertexArrays(n, handles); }

        inline auto gen_buffers(GLsizei n, GLuint *handles) { return glGenBuffers(n, handles); }
        inline auto del_buffers(GLsizei n, const GLuint *handles) { return glDeleteBuffers(n, handles); }

        inline auto gen_textures(GLsizei n, GLuint *handles) { return glGenTextures(n, handles); }
        inline auto del_textures(GLsizei n, const GLuint *handles) { return glDeleteTextures(n, handles); }
    }

    using vao = GLObj<detail::gen_vertex_arrays, detail::delete_vertex_arrays>;
    using vbo = GLObj<detail::gen_buffers, detail::del_buffers>;
    using texture = GLObj<detail::gen_textures, detail::del_textures>;
}}
