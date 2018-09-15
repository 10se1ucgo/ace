#include "draw/debug.h"
#include "pybind11/pybind11.h"

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

    // py::class_<VXLBlocks>(m, "VXLBlocks")
}