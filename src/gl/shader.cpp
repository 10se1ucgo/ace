#include "gl/shader.h"

#include "common.h"
#include "util/except.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <memory>
#include <fstream>


namespace ace { namespace gl {
    Shader::Shader(const std::string &file, GLenum type): handle(glCreateShader(type)) {
        std::ifstream in(file, std::ios::in | std::ios::binary);
        if(in.fail()) {
            THROW_ERROR("Could not read shader file {}", file);
        }
        std::string source{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};

        const GLchar *source_buff = source.c_str();
        glShaderSource(this->handle, 1, &source_buff, nullptr);
        glCompileShader(this->handle);

        GLint status;
        glGetShaderiv(this->handle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length;
            glGetShaderiv(this->handle, GL_INFO_LOG_LENGTH, &length);
            auto log(std::make_unique<GLchar[]>(length));
            glGetShaderInfoLog(this->handle, length, nullptr, log.get());
            THROW_ERROR("Error compiling shader: {}", log.get());
        }
    }

    Shader::~Shader() {
        glDeleteShader(this->handle);
    }

    GLuint ShaderProgram::bound_program = 0;

    ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders) : program(glCreateProgram()) {
        for (const Shader &s : shaders) {
            glAttachShader(this->program, s.handle);
        }

        glLinkProgram(this->program);

        GLint status;
        glGetProgramiv(this->program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length;
            glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &length);
            auto log = std::make_unique<GLchar[]>(length);
            glGetShaderInfoLog(this->program, length, nullptr, log.get());
            THROW_ERROR("Error linking shader program: {}", log.get());
        }
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(this->program);
    }

    ShaderManager::ShaderManager():
        model({
            { get_resource_path("shaders/model.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/model.frag"), GL_FRAGMENT_SHADER }
        }), 
        map({
            { get_resource_path("shaders/map.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/map.frag"), GL_FRAGMENT_SHADER }
        }), 
        sprite({
            { get_resource_path("shaders/sprite.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/sprite.frag"), GL_FRAGMENT_SHADER }
        }),
        billboard({
            { get_resource_path("shaders/bb.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/bb.geom"), GL_GEOMETRY_SHADER },
            { get_resource_path("shaders/bb.frag"), GL_FRAGMENT_SHADER }
        }), 
        text({
            { get_resource_path("shaders/text.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/text.frag"), GL_FRAGMENT_SHADER }
        }),
        line({
            { get_resource_path("shaders/line.vert"), GL_VERTEX_SHADER },
            { get_resource_path("shaders/line.frag"), GL_FRAGMENT_SHADER }
        }) {
    }
}}

//void ShaderProgram::uniform(const std::string &name, glm::mat4 &mat, bool transpose) {
//    const GLint location = this->uniform_loc(name);
//    glUniformMatrix4fv(location, 1, transpose, value_ptr(mat));
//}



#undef DECLARE_UNIFORM_V_FUNC