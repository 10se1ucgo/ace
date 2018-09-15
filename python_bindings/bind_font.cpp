#include "pybind11/pybind11.h"
#include "draw/font.h"


namespace py = pybind11;
using namespace py::literals;


void bind_font(py::module &a) {
    auto m(a.def_submodule("font"));

    using namespace ace::draw;

    py::enum_<ace::draw::Align>(m, "Align")
        .value("TOP_LEFT", Align::TOP_LEFT)
        .value("BOTTOM_LEFT", Align::BOTTOM_LEFT)
        .value("TOP_RIGHT", Align::TOP_RIGHT)
        .value("BOTTOM_RIGHT", Align::BOTTOM_RIGHT)
        .value("CENTER", Align::CENTER)
        .value("TOP_CENTER", Align::TOP_CENTER)
        .value("BOTTOM_CENTER", Align::BOTTOM_CENTER);

    py::class_<Font>(m, "Font")
        .def("measure", &Font::measure)
        .def("draw", py::overload_cast<const std::string &, glm::vec2, glm::vec3, glm::vec2, Align>(&Font::draw),
             "str"_a, "pos"_a, "color"_a = glm::vec3(1.0f), "scale"_a = glm::vec2(1.f), "align"_a = Align::BOTTOM_LEFT, py::call_guard<py::gil_scoped_release>())
        .def("draw_shadowed", py::overload_cast<const std::string &, glm::vec2, glm::vec3, glm::vec2, Align>(&Font::draw_shadowed),
             "str"_a, "pos"_a, "color"_a = glm::vec3(1.0f), "scale"_a = glm::vec2(1.f), "align"_a = Align::BOTTOM_LEFT, py::call_guard<py::gil_scoped_release>())
        .def_property_readonly("size", &Font::size)
        .def("get_aligned_position", &Font::get_aligned_position);

    py::class_<Text>(m, "Label")
        .def(py::init<Font *, std::string, glm::vec3, glm::vec2, Align>(),
             "font"_a, "str"_a, "color"_a = glm::vec3(1.0f), "scale"_a = glm::vec2(1.f), "align"_a = Align::BOTTOM_LEFT, py::call_guard<py::gil_scoped_release>())
        .def("update", py::overload_cast<std::string, glm::vec3, glm::vec2, Align>(&Text::update),
             "str"_a, "color"_a = glm::vec3(1.0f), "scale"_a = glm::vec2(1.f), "align"_a = Align::BOTTOM_LEFT, py::call_guard<py::gil_scoped_release>())
        .def("draw", &Text::draw, py::call_guard<py::gil_scoped_release>())
        .def("draw_shadowed", &Text::draw_shadowed, py::call_guard<py::gil_scoped_release>())
        .def("get_aligned", py::overload_cast<>(&Text::get_position, py::const_))
        .def("get_aligned", py::overload_cast<Align>(&Text::get_position, py::const_))
        .def_property("str", &Text::str, &Text::set_str, py::call_guard<py::gil_scoped_release>())
        .def_property("color", &Text::color, &Text::set_color, py::call_guard<py::gil_scoped_release>())
        .def_property("scale", &Text::scale, &Text::set_scale, py::call_guard<py::gil_scoped_release>())
        .def_property("align", &Text::alignment, &Text::set_alignment, py::call_guard<py::gil_scoped_release>())
        .def_readwrite("position", &Text::position);
}