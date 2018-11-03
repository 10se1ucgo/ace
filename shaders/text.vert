#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;
layout (location = 2) in vec4 tex_coords;
layout (location = 3) in vec3 color;

out TextGlyph {
    vec2 position;
    vec2 size;
    vec4 tex_coords;
    vec3 color;
} glyph;

void main() {
    glyph.position = position;
    glyph.size = size;
    glyph.tex_coords = tex_coords;
    glyph.color = color;
}