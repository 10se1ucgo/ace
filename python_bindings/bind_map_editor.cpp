#include "bindings.h"
#include <pybind11/stl.h>
#include "scene/map_editor.h"
#include "game_client.h"

namespace py = pybind11;
using namespace py::literals;



void bind_map_editor(py::module &a) {
    using namespace ace;

    auto m(a.def_submodule("editor"));

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
            // .def_readwrite("color", &scene::MapEditor::color)
            .def("hitcast", [](const scene::MapEditor &self) {
                glm::ivec3 hit;
                ace::Face f = self.hitcast(&hit);
                return py::make_tuple(f, hit);
            }, py::call_guard<py::gil_scoped_release>())
            .def("fonts", [](scene::MapEditor &self, const std::string &name, int size) {
                return self.fonts.get(name, size);
            }, py::return_value_policy::reference_internal, py::call_guard<py::gil_scoped_release>())
            .def_property_readonly("client", [](scene::MapEditor &self) -> auto& { return self.client; }, py::return_value_policy::reference_internal);






            // .def("sprites", [](scene::MapEditor &self, const std::string &name) {
            //     return self.client.sprites.get(name);
            // }, py::return_value_policy::reference_internal);

        

    // TODO
}