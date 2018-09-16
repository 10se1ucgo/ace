#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "common.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gl_util.h"


#define DECLARE_UNIFORM_FUNC(T_TYPE, T_FUNC) \
void uniform(const std::string &name, T_TYPE v0) { return glUniform1##T_FUNC(this->uniform_loc(name), v0); } \
void uniform(const std::string &name, T_TYPE v0, T_TYPE v1) { return glUniform2##T_FUNC(this->uniform_loc(name), v0, v1); } \
void uniform(const std::string &name, T_TYPE v0, T_TYPE v1, T_TYPE v2) { return glUniform3##T_FUNC(this->uniform_loc(name), v0, v1, v2); } \
void uniform(const std::string &name, T_TYPE v0, T_TYPE v1, T_TYPE v2, T_TYPE v3) { return glUniform4##T_FUNC(this->uniform_loc(name), v0, v1, v2, v3); }

#define DECLARE_UNIFORM_M_FUNC(T_DIM) \
void uniform(const std::string &name, const glm::mat##T_DIM &mat, bool transpose=false) { \
    const GLint location = this->uniform_loc(name); \
    glUniformMatrix##T_DIM##fv(location, 1, transpose, value_ptr(mat)); \
}

#define DECLARE_UNIFORM_V_FUNC(T_TYPE, T_FUNC) \
void uniform(const std::string &name, const T_TYPE##2 &vec) { glUniform2##T_FUNC##v(this->uniform_loc(name), 1, value_ptr(vec)); } \
void uniform(const std::string &name, const T_TYPE##3 &vec) { glUniform3##T_FUNC##v(this->uniform_loc(name), 1, value_ptr(vec)); } \
void uniform(const std::string &name, const T_TYPE##4 &vec) { glUniform4##T_FUNC##v(this->uniform_loc(name), 1, value_ptr(vec)); } \


namespace ace { namespace gl {
    template<typename T>
    using uni_arg = std::pair<const char *, T>;

    // inspired by fmtlib UDL syntax :') 
    struct UniformArgument {
        const char *str;

        template <typename T>
        uni_arg<T> operator=(T &&value) const {
            return { str, std::forward<T>(value) };
        }
    };

    inline namespace literals {
        inline UniformArgument operator""_u(const char *s, std::size_t) { return UniformArgument{ s }; }
    }

    struct Shader {
        Shader(const std::string &file, GLenum type);
        ~Shader();
        ACE_NO_COPY_MOVE(Shader)

        GLuint handle;
    };

    struct ShaderProgram {
        ShaderProgram(std::initializer_list<Shader> shaders);
        ~ShaderProgram();
        ACE_NO_COPY_MOVE(ShaderProgram)

        void bind() const {
            if (bound_program == this->program) return;
            glUseProgram(this->program);
            bound_program = this->program;
        }
        static void unbind() { glUseProgram(0); }

        DECLARE_UNIFORM_FUNC(GLint, i)
        DECLARE_UNIFORM_FUNC(GLuint, ui)
        DECLARE_UNIFORM_FUNC(GLfloat, f)

        DECLARE_UNIFORM_M_FUNC(2)
        DECLARE_UNIFORM_M_FUNC(3)
        DECLARE_UNIFORM_M_FUNC(4)
        DECLARE_UNIFORM_M_FUNC(2x3)
        DECLARE_UNIFORM_M_FUNC(3x2)
        DECLARE_UNIFORM_M_FUNC(2x4)
        DECLARE_UNIFORM_M_FUNC(4x2)
        DECLARE_UNIFORM_M_FUNC(3x4)
        DECLARE_UNIFORM_M_FUNC(4x3)

        DECLARE_UNIFORM_V_FUNC(glm::ivec, i)
        DECLARE_UNIFORM_V_FUNC(glm::uvec, ui)
        DECLARE_UNIFORM_V_FUNC(glm::fvec, f)

        template<typename THead, typename... TTail>
        void uniforms(const uni_arg<THead> &head, const uni_arg<TTail> & ...uniforms) {
            this->uniform(std::string(head.first), head.second);
            return this->uniforms(uniforms...);
        }

        GLuint uniform_loc(const std::string &name) {
            try {
                return uniform_cache.at(name);
            } catch (const std::out_of_range &) {
                const GLuint loc = glGetUniformLocation(this->program, name.c_str());
                this->uniform_cache[name] = loc;
                fmt::print("Loaded uniform location {}@{}\n", name, loc);
                return loc;
            }
        }

        GLuint program;
        std::unordered_map<std::string, GLuint> uniform_cache;

        static GLuint bound_program;

    private:
        void uniforms() { }
    };

    namespace experimental {
        template<typename T>
        struct ubo {
            explicit ubo(GLenum usage = GL_DYNAMIC_DRAW) {
                this->bind();
                glBufferData(GL_UNIFORM_BUFFER, sizeof(this->data), nullptr, usage);
            }

            void bind() const {
                glBindBuffer(GL_UNIFORM_BUFFER, this->handle);
            }

            void upload() {
                this->bind();
                void *buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
                memcpy(buffer, &this->data, sizeof(this->data));
                glUnmapBuffer(GL_UNIFORM_BUFFER);
            }

            T *operator->() { return &this->data; }

            gl::buffer handle;
            T data;
        };
    }

    struct ShaderManager {
        ShaderManager();

        template<typename T>
        experimental::ubo<T> create_ubo(const std::string &name, int index = 0) {
            experimental::ubo<T> ubo;
            glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo.handle);

            GLint num;
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &num);
            fmt::print("INDEX: {}, MAX: {}\n", index, num);

            for (ShaderProgram *s : { &model, &map, &sprite, &billboard, &text }) {
                auto bi = glGetUniformBlockIndex(s->program, name.c_str());
                if (bi == GL_INVALID_INDEX) continue;
                fmt::print("BI: {}\n", bi);
                glUniformBlockBinding(s->program, bi, index);
            }

            return ubo;
        }
        
        ShaderProgram model, map, sprite, billboard, text, line;
    };

    #undef DECLARE_UNIFORM
    #undef DECLARE_UNIFORM_M_FUNC
    #undef DECLARE_UNIFORM_V_FUNC
}}