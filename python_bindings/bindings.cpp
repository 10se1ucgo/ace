#include "bindings.h"

#include <pybind11/embed.h>

PYBIND11_EMBEDDED_MODULE(ace, m) {
    bind_sdl(m);
    bind_gl(m);
    bind_glm(m);
    bind_font(m);
    bind_draw(m);
    bind_map(m);
    bind_map_editor(m);
    bind_client(m);
}