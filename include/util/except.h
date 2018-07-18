#pragma once
#include <stdexcept>

#include "fmt/format.h"
#include "glad/glad.h"

namespace ace {
    class RuntimeException: public std::runtime_error {
    public:
        template <typename... T>
        RuntimeException(const char *file, size_t line, const char *function, const char *format, const T &...args) :
            std::runtime_error(fmt::format("Error near {0}:{1}, in function {2}\n{3}", file, line, function, fmt::format(format, args...))) {}
    };

    static const char *get_gl_debug_source_name(GLenum source) {
        switch(source) {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "<UNKNOWN>";
        }
        
    }

    static const char *get_gl_debug_type_name(GLenum type) {
        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        default:
            return "<UNKNOWN>";
        }
    }

    static const char *get_gl_debug_severity_name(GLenum severity) {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        default:
            return "<UNKNOWN>";
        }
    }
}


#define THROW_ERROR(fmt, ...) throw ace::RuntimeException(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

