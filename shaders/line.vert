#version 330 core
// could just be a basic v3c4 shader tbh
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out vec4 f_color;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    f_color = color;
}