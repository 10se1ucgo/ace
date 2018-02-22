#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

layout (location = 2) in vec4 tint;
layout (location = 3) in mat3 model;
// 3 = col 0, 4 = col 1, 5 = col 2

out vec2 f_tex;
out vec4 f_tint;

uniform mat4 projection;

void main() {
    gl_Position = (projection * mat4(model)) * vec4(pos, 1.0);
    f_tex = tex;
    f_tint = tint;
}