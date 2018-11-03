#version 330 core
in vec2 f_tex_coords;
in vec3 f_color;
out vec4 frag_color;

uniform sampler2D tex;

void main() {
    frag_color = vec4(1, 1, 1, texture(tex, f_tex_coords).r) * vec4(f_color, 1);
}