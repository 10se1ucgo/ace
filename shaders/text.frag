#version 330 core
in vec2 tex_coords;
in vec4 f_color;
out vec4 frag_color;

uniform sampler2D tex;

void main() {
    frag_color = vec4(1, 1, 1, texture(tex, tex_coords).r) * f_color;
}