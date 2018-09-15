#include "gl/shader.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;
using namespace py::literals;


#define BIND_UNIFORM_FUNC(T_TYPE) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE>(&ShaderProgram::uniform)) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE>(&ShaderProgram::uniform)) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE, T_TYPE>(&ShaderProgram::uniform)) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE, T_TYPE, T_TYPE>(&ShaderProgram::uniform)) \

#define BIND_UNIFORM_MAT(T_DIM) \
    .def("uniform", py::overload_cast<const std::string &, const glm::mat##T_DIM &, bool>(&ShaderProgram::uniform), "name"_a, "mat"_a, "transpose"_a = false)

#define BIND_UNIFORM_VEC(T_TYPE) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##2 &>(&ShaderProgram::uniform)) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##3 &>(&ShaderProgram::uniform)) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##4 &>(&ShaderProgram::uniform)) \

void bind_gl(py::module &a) {
    auto m(a.def_submodule("gl"));
    using namespace ace::gl;

    py::class_<ShaderProgram>(m, "ShaderProgram")
        .def("bind", &ShaderProgram::bind)
        .def("unbind", &ShaderProgram::unbind)
        // .def("uniforms", [](ShaderProgram &self, py::kwargs kwargs){})
        BIND_UNIFORM_MAT(3)
        BIND_UNIFORM_MAT(4)
        BIND_UNIFORM_FUNC(GLint)
        BIND_UNIFORM_FUNC(GLuint)
        BIND_UNIFORM_FUNC(GLfloat)
        BIND_UNIFORM_VEC(glm::ivec)
        BIND_UNIFORM_VEC(glm::uvec)
        BIND_UNIFORM_VEC(glm::fvec);
}

#undef BIND_UNIFORM_FUNC
#undef BIND_UNIFORM_MAT
#undef BIND_UNIFORM_VEC