#include "vxl.h"
#include "draw/map.h"
#include "bindings.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace py::literals;


void bind_map(py::module &a) {
    auto m(a.def_submodule("map", "Python bindings to AceMap =)"));

    using ace::EditableMap;

    m.attr("MAP_X") = ace::MAP_X;
    m.attr("MAP_Y") = ace::MAP_Y;
    m.attr("MAP_Z") = ace::MAP_Z;
    m.def("get_pos", &ace::get_pos, py::call_guard<py::gil_scoped_release>());
    m.def("is_valid_pos", py::overload_cast<int, int, int>(&ace::is_valid_pos), py::call_guard<py::gil_scoped_release>());
    m.def("is_valid_pos", py::overload_cast<size_t>(&ace::is_valid_pos), py::call_guard<py::gil_scoped_release>());
    m.def("is_valid_pos", py::overload_cast<glm::ivec3>(&ace::is_valid_pos), py::call_guard<py::gil_scoped_release>());
    m.def("jit_color", &ace::jit_color, py::call_guard<py::gil_scoped_release>());
    m.def("vox2draw", &ace::vox2draw, py::call_guard<py::gil_scoped_release>());
    m.def("draw2vox", &ace::draw2vox, py::call_guard<py::gil_scoped_release>());

    pyenum34<ace::Face>(m, "Face")
        .value("INVALID", ace::Face::INVALID)
        .value("LEFT", ace::Face::LEFT)
        .value("RIGHT", ace::Face::RIGHT)
        .value("BACK", ace::Face::BACK)
        .value("FRONT", ace::Face::FRONT)
        .value("TOP", ace::Face::TOP)
        .value("BOTTOM", ace::Face::BOTTOM)
        .finish();

    m.def("next_block", ace::draw::next_block);
    m.def("get_face", ace::draw::get_face);
    m.def("block_line", py::overload_cast<const glm::ivec3, const glm::ivec3>(ace::block_line), py::call_guard<py::gil_scoped_release>());

    py::class_<EditableMap>(m, "AceMap")
        .def("set_block", [](EditableMap &self, int x, int y, int z, bool solid, glm::u8vec3 color, bool wrapped) {
            self.set_block(x, y, z, solid, glm::u8vec4(color, 0x7F), wrapped);
        }, "x"_a, "y"_a, "z"_a, "solid"_a, "color"_a = glm::u8vec3(0), "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("get_block", [](EditableMap &self, int x, int y, int z, bool wrapped) {
            glm::u8vec4 color;
            bool solid = self.get_block(x, y, z, &color, wrapped);
            return py::make_tuple(solid, color);
        }, "x"_a, "y"_a, "z"_a, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())

        .def("set_solid", &EditableMap::set_solid, "x"_a, "y"_a, "z"_a, "solid"_a, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("is_solid", &EditableMap::is_solid, "x"_a, "y"_a, "z"_a, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())

        .def("set_color", [](EditableMap &self, int x, int y, int z, glm::u8vec3 color, bool wrapped) {
            self.set_color(x, y, z, glm::u8vec4(color, 0x7F), wrapped);
        }, "x"_a, "y"_a, "z"_a, "color"_a = glm::u8vec3(0), "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("get_color", [](EditableMap &self, int x, int y, int z, bool wrapped) {
            return self.get_color(x, y, z, wrapped);
        }, "x"_a, "y"_a, "z"_a, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("get_z", &EditableMap::get_z, "x"_a, "y"_a, "start"_a = 0, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("get_vis", &EditableMap::get_vis, "x"_a, "y"_a, "z"_a, "wrapped"_a = false, py::call_guard<py::gil_scoped_release>())
        .def("hitscan", [](EditableMap &self, glm::vec3 p, glm::vec3 d) {
            glm::ivec3 hit;
            ace::Face f = self.hitscan(p, d, &hit);
            return py::make_tuple(f, hit);
        }, py::call_guard<py::gil_scoped_release>());
}
