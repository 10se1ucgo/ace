#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec3 color;
out vec4 f_color;
out vec2 f_tex_coords;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    f_tex_coords = tex_coords;
    f_color = vec4(color, 1.0);
}