#include "game_client.h"
#include "gl/shader.h"
#include "bindings.h"


namespace py = pybind11;
using namespace py::literals;



void bind_client(py::module &m) {
    using namespace ace;


    py::class_<GameClient>(m, "Client")
        .def("quit", &GameClient::quit)
        .def("size", &GameClient::size)
        .def("width", &GameClient::width)
        .def("height", &GameClient::height)
        // .def("set_exclusive_mouse", &GameClient::set_exclusive_mouse)
        .def_property("exclusive_mouse", &GameClient::exclusive_mouse, &GameClient::set_exclusive_mouse, py::call_guard<py::gil_scoped_release>())
        .def_property("text_input_active", &GameClient::text_input_active, [](GameClient &self, bool active) {
            if(self.text_input_active() != active) {
                self.toggle_text_input();
            }
        }, py::call_guard<py::gil_scoped_release>())
        .def_readwrite("input_buffer", &GameClient::input_buffer)
        .def_readwrite("input_cursor", &GameClient::input_cursor);
    // .def_property_readonly("shaders", [](const GameClient &self) -> gl::ShaderManager * { return self.shaders.get(); }, py::call_guard<py::gil_scoped_release>(), py::return_value_policy::reference_internal)
    // idk why but this doesnt want to compile because of some std::pair stuff (the unordered_map storing unique_ptrs to shaders)
}