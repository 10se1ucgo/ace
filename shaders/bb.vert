#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in float size;

out Billboard {
    vec3 pos;
    vec3 color;
    float size;
} bb;

void main() {
    gl_Position = vec4(0, 0, 0, 1.0);
    bb.pos = pos;
    bb.color = color;
    bb.size = size;
}