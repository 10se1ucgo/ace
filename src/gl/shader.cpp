#include "gl/shader.h"

#include "common.h"
#include "util/except.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <memory>
#include <fstream>


namespace ace { namespace gl {
    namespace {
        bool read_file(const std::string &file_path, std::string &buffer) {
            std::ifstream in(file_path, std::ios::in | std::ios::binary);
            if (in.fail()) return false;

            buffer = { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
            return true;
        }
    }


    Shader::Shader(const std::string &source, GLenum type): handle(glCreateShader(type)) {
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

    ShaderProgram::ShaderProgram(): program(glCreateProgram()) {
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(this->program);
    }

    void ShaderProgram::add(const Shader &s) {
        glAttachShader(this->program, s.handle);
    }

    void ShaderProgram::link() {
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

    ShaderManager::ShaderManager() :
        model(this->get("model")),
        map(this->get("map")),
        sprite(this->get("sprite")),
        billboard(this->get("bb")),
        text(this->get("text")),
        line(this->get("line")) {
    }

    ShaderProgram &ShaderManager::get(const std::string &name) {
        try {
            return *this->programs.at(name);
        } catch (std::out_of_range &) {
            auto path(get_resource_path("shaders/" + name));
        
            auto program(std::make_unique<ShaderProgram>());
        
            std::string source_buffer;
        
            if (read_file(path + ".vert", source_buffer))
                program->add(Shader{ source_buffer, GL_VERTEX_SHADER });
            if (read_file(path + ".frag", source_buffer))
                program->add(Shader{ source_buffer, GL_FRAGMENT_SHADER });
            if (read_file(path + ".geom", source_buffer))
                program->add(Shader{ source_buffer, GL_GEOMETRY_SHADER });

            program->link();
            
            return *this->programs.emplace(name, std::move(program)).first->second;
        }
    }
}}

//void ShaderProgram::uniform(const std::string &name, glm::mat4 &mat, bool transpose) {
//    const GLint location = this->uniform_loc(name);
//    glUniformMatrix4fv(location, 1, transpose, value_ptr(mat));
//}



#undef DECLARE_UNIFORM_V_FUNC