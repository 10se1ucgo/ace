#version 450 core
layout (location = 0) in vec4 coords;
layout (location = 1) in vec4 color;
out vec4 f_color;
out vec2 tex_coords;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(coords.xy, 0.0, 1.0);
    tex_coords = coords.zw;
    f_color = color;
}