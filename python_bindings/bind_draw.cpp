#include "draw/debug.h"
#include "draw/map.h"
#include "bindings.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace py::literals;



void bind_draw(py::module &a) {
    auto m(a.def_submodule("draw"));
    using namespace ace::draw;

    py::class_<DebugDraw>(m, "DebugDraw")
        .def("draw_line", &DebugDraw::draw_line, py::call_guard<py::gil_scoped_release>())
        .def("draw_ray", &DebugDraw::draw_ray, py::call_guard<py::gil_scoped_release>())
        .def("draw_triangle", &DebugDraw::draw_triangle, py::call_guard<py::gil_scoped_release>())
        .def("draw_quad", &DebugDraw::draw_quad, py::call_guard<py::gil_scoped_release>())
        .def("draw_cube", &DebugDraw::draw_cube, py::call_guard<py::gil_scoped_release>());

    py::class_<VXLBlocks>(m, "VXLBlocks")
        .def(py::init<glm::u8vec3>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<const std::unordered_map<glm::ivec3, glm::u8vec3> &, const glm::vec3 &>(), py::call_guard<py::gil_scoped_release>())
        .def("update", py::overload_cast<const std::unordered_map<glm::ivec3, glm::u8vec3> &, const glm::vec3 &>(&VXLBlocks::update), py::call_guard<py::gil_scoped_release>())
        .def("draw", &VXLBlocks::draw, py::call_guard<py::gil_scoped_release>())
        .def_readwrite("scale", &VXLBlocks::scale)
        .def_readwrite("rotation", &VXLBlocks::rotation)
        .def_readwrite("position", &VXLBlocks::position)
        .def_readwrite("centroid", &VXLBlocks::centroid);
}