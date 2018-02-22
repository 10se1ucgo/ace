#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in uint face;

out vec3 color;
out float fog;

uniform mat4 mvp;

uniform vec3 filter_color = vec3(0.0);
uniform vec3 replacement_color = vec3(0.0);

const float shading[6] = float[](
    0.75,
    0.75,
    0.875,
    0.625,
    1.0,
    0.5
);

void main() {
    gl_Position = mvp * vec4(pos, 1.0);
    color = (vertex_color == filter_color ? replacement_color : vertex_color) * shading[face];
    fog = 1.0 - clamp((128 - length(gl_Position.xyz)) / 64, 0.0, 1.0);
}