#pragma once
#include "common.h"
#include "gl/gl_obj.h"

#include "glm/glm.hpp"
#include "glad/glad.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

struct Shader {
    Shader(const std::string &file, GLenum type);
    ~Shader();
    ACE_NO_COPY_MOVE(Shader)

    GLuint handle;
};

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

    void uniform(const std::string &name, std::initializer_list<GLint> values);
    void uniform(const std::string &name, std::initializer_list<GLuint> values);
    void uniform(const std::string &name, std::initializer_list<GLfloat> values);

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

    GLuint uniform_loc(const std::string &name) {
        try {
            return uniform_cache.at(name);
        } catch (const std::out_of_range &) {
            const GLuint loc = glGetUniformLocation(this->program, name.c_str());
            this->uniform_cache[name] = loc;
            return loc;
        }
        
    }

    GLuint program;
    std::unordered_map<std::string, GLuint> uniform_cache;

    static GLuint bound_program;
};


#undef DECLARE_UNIFORM
#undef DECLARE_UNIFORM_M_FUNC
#undef DECLARE_UNIFORM_V_FUNC

struct ShaderManager {
    ShaderManager();

    ShaderProgram model, map, sprite, billboard, text;
};

