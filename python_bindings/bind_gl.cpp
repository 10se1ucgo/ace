#include "gl/shader.h"
#include "bindings.h"
#include "scene/scene.h"

namespace py = pybind11;
using namespace py::literals;


#define BIND_UNIFORM_FUNC(T_TYPE) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>()) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>()) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE, T_TYPE>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>()) \
    .def("uniform", py::overload_cast<const std::string &, T_TYPE, T_TYPE, T_TYPE, T_TYPE>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>())

#define BIND_UNIFORM_MAT(T_DIM) \
    .def("uniform", py::overload_cast<const std::string &, const glm::mat##T_DIM &, bool>(&ShaderProgram::uniform), "name"_a, "mat"_a, "transpose"_a = false, py::call_guard<py::gil_scoped_release>())

#define BIND_UNIFORM_VEC(T_TYPE) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##2 &>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>()) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##3 &>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>()) \
    .def("uniform", py::overload_cast<const std::string &, const T_TYPE##4 &>(&ShaderProgram::uniform), py::call_guard<py::gil_scoped_release>())

void bind_gl(py::module &a) {
    auto m(a.def_submodule("gl"));
    using namespace ace::gl;

    py::class_<ShaderManager>(m, "ShaderManager")
        .def_property_readonly("model", [](ShaderManager &self) { return &self.model; })
        .def_property_readonly("map", [](ShaderManager &self) { return &self.map; })
        .def_property_readonly("sprite", [](ShaderManager &self) { return &self.sprite; })
        .def_property_readonly("billboard", [](ShaderManager &self) { return &self.billboard; })
        .def_property_readonly("text", [](ShaderManager &self) { return &self.text; })
        .def_property_readonly("line", [](ShaderManager &self) { return &self.line; });


    py::class_<ShaderProgram>(m, "ShaderProgram")
        .def("bind", &ShaderProgram::bind, py::call_guard<py::gil_scoped_release>())
        .def("unbind", &ShaderProgram::unbind, py::call_guard<py::gil_scoped_release>())
        // TODO .def("uniforms", [](ShaderProgram &self, py::kwargs kwargs){})
        BIND_UNIFORM_MAT(3)
        BIND_UNIFORM_MAT(4)
        BIND_UNIFORM_FUNC(GLint)
        BIND_UNIFORM_FUNC(GLuint)
        BIND_UNIFORM_FUNC(GLfloat)
        BIND_UNIFORM_VEC(glm::ivec)
        BIND_UNIFORM_VEC(glm::uvec)
        BIND_UNIFORM_VEC(glm::fvec);

    using unis = ace::scene::Uniforms3D;
    py::class_<unis>(m, "Uniforms3D")
        .def_readwrite("view", &unis::view)
        .def_readwrite("proj", &unis::proj)
        .def_readwrite("pv", &unis::pv)
        .def_readwrite("cam_pos", &unis::cam_pos)
        .def_readwrite("cam_forward", &unis::cam_forward)
        .def_readwrite("cam_right", &unis::cam_right)
        .def_readwrite("cam_up", &unis::cam_up)
        .def_readwrite("fog_color", &unis::fog_color)
        .def_readwrite("light_pos", &unis::light_pos)
        .def_readwrite("fog_start", &unis::fog_start)
        .def_readwrite("fog_end", &unis::fog_end);


    // TODO i just shoved these here cause I need them for transparency. 
    // I need to probably add some sort of GL state manager nowish.
    m.def("Blend", [](bool blend) {
        if (blend) 
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    });
    m.def("DepthTest", [](bool depth_test) {
        if (depth_test)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    });
    m.def("DepthMask", [](bool mask) {
        glDepthMask(mask);
    });
}

#undef BIND_UNIFORM_FUNC
#undef BIND_UNIFORM_MAT
#undef BIND_UNIFORM_VEC