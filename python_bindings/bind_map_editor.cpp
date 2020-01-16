#include "bindings.h"
#include <pybind11/stl.h>
#include "scene/map_editor.h"
#include "game_client.h"

namespace py = pybind11;
using namespace py::literals;



void bind_map_editor(py::module &a) {
    using namespace ace;

    auto m(a.def_submodule("editor"));

    m.def("kv6_to_blocks", &scene::kv6_to_blocks);

    py::class_<scene::BlockSelection>(m, "BlockSelection")
        .def("get_blocks", &scene::BlockSelection::get_blocks, py::call_guard<py::gil_scoped_release>(), py::return_value_policy::reference_internal)
        .def("build", py::overload_cast<glm::u8vec3>(&scene::BlockSelection::build), py::call_guard<py::gil_scoped_release>())
        .def("build", py::overload_cast<glm::u8vec3, Face, int>(&scene::BlockSelection::build), "color"_a, "face"_a, "depth"_a = 1,  py::call_guard<py::gil_scoped_release>())
        .def("destroy", py::overload_cast<>(&scene::BlockSelection::destroy), py::call_guard<py::gil_scoped_release>())
        .def("destroy", py::overload_cast<Face>(&scene::BlockSelection::destroy), py::call_guard<py::gil_scoped_release>())
        .def("paint", py::overload_cast<glm::u8vec3>(&scene::BlockSelection::paint), py::call_guard<py::gil_scoped_release>())
        .def("paint", py::overload_cast<glm::u8vec3, Face>(&scene::BlockSelection::paint), py::call_guard<py::gil_scoped_release>());

    py::class_<scene::LineSelection, scene::BlockSelection>(m, "LineSelection")
        .def(py::init<EditableMap &>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<EditableMap &, glm::ivec3, glm::ivec3>(), py::call_guard<py::gil_scoped_release>())
        .def("set", &scene::LineSelection::set, py::call_guard<py::gil_scoped_release>());

    py::class_<scene::BoxSelection, scene::BlockSelection>(m, "BoxSelection")
        .def(py::init<EditableMap &>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<EditableMap &, glm::ivec3, glm::ivec3>(), py::call_guard<py::gil_scoped_release>())
        .def("set", &scene::BoxSelection::set, py::call_guard<py::gil_scoped_release>());

    py::class_<scene::FaceSelection, scene::BlockSelection>(m, "FaceSelection")
        .def(py::init<EditableMap &>(), py::call_guard<py::gil_scoped_release>())
        .def("set", &scene::FaceSelection::set, py::call_guard<py::gil_scoped_release>());

    py::class_<scene::MapEditor>(m, "MapEditor")
        .def_property_readonly("map", [](scene::MapEditor &self) -> auto& {
            return self.map;
        })
        .def_property_readonly("debug", [](scene::MapEditor &self) -> auto& {
            return self.debug;
        })
        .def_property_readonly("uniforms", [](scene::MapEditor &self) -> auto& {
            return self.uniforms.data;
        })
        .def("hitcast", [](const scene::MapEditor &self) {
            glm::ivec3 hit;
            ace::Face f = self.hitcast(&hit);
            return py::make_tuple(f, hit);
        }, py::call_guard<py::gil_scoped_release>())
        .def("save_to", &scene::MapEditor::save_to, py::call_guard<py::gil_scoped_release>())
        .def("fonts", [](scene::MapEditor &self, const std::string &name, int size) {
            return self.fonts.get(name, size);
        }, py::return_value_policy::reference_internal, py::call_guard<py::gil_scoped_release>())
        .def("kv6_to_blocks", [](scene::MapEditor &self, const std::string &kv6name) {
            return scene::kv6_to_blocks(self.client.models.get(kv6name)->data);
        })
        .def_property_readonly("client", [](scene::MapEditor &self) -> auto& { return self.client; }, py::return_value_policy::reference_internal);





        // .def("sprites", [](scene::MapEditor &self, const std::string &name) {
        //     return self.client.sprites.get(name);
        // }, py::return_value_policy::reference_internal);

        

    // TODO
}