#version 330 core
in vec3 color;
in float diffuse;
in float fog;

out vec4 frag_color;

layout (std140) uniform SceneUniforms {
    mat4 view;
    mat4 proj;
    mat4 pv;
    vec3 cam_forward;
    vec3 cam_right;
    vec3 cam_up;
    vec3 fog_color;
    vec3 light_pos;
};

void main() {
    frag_color = vec4(mix(color * (1 + 0.4 * diffuse), fog_color, fog), 1.0);
}