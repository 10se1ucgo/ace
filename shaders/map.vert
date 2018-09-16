#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in uint face;
layout (location = 3) in uint ao_level;

out vec3 color;
out float ao_mult;
out float fog;

layout (std140) uniform SceneUniforms {
    mat4 view;
    mat4 proj;
    mat4 pv;
    vec3 cam_pos;
    vec3 cam_forward;
    vec3 cam_right;
    vec3 cam_up;
    vec3 fog_color;
    vec3 light_pos;
    float fog_start;
    float fog_end;
};

uniform mat4 model;

uniform vec3 filter_color;
uniform vec3 replacement_color;

const float shading[6] = float[](
    0.75,
    0.75,
    0.875,
    0.625,
    1.0,
    0.5
);

const float ao[4] = float[](
    1.0,
    0.9,
    0.8,
    0.6
);

void main() {
    vec4 view_space = view * model * vec4(pos, 1.0);
    gl_Position = proj * view_space;
    color = (vertex_color == filter_color ? replacement_color : vertex_color) * shading[face];
    ao_mult = ao[ao_level];
    fog = 1.0 - clamp((fog_end - length(view_space)) / fog_start, 0.0, 1.0);
}