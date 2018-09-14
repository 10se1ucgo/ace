#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bind_sdl(py::module &);
void bind_gl(py::module &);
void bind_glm(py::module &);
void bind_font(py::module &);
void bind_draw(py::module &);
void bind_map(py::module &);
void bind_map_editor(py::module &);

PYBIND11_EMBEDDED_MODULE(ace, m) {
    m.doc() = "ace engine";
    bind_sdl(m);
    bind_gl(m);
    bind_glm(m);
    bind_font(m);
    bind_draw(m);
    bind_map(m);
    bind_map_editor(m);
}