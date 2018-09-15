#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace py::literals;



void bind_map_editor(py::module &a) {
    auto m(a.def_submodule("editor"));

    // TODO
}