#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in Billboard {
    vec3 pos;
    vec3 color;
    float size;
} bb[];

out vec3 frag_color;

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


void main() {
    gl_Position = pv * vec4(((cam_right - cam_up) * bb[0].size) + bb[0].pos, 1.0); // bottom left
    frag_color = bb[0].color;
    EmitVertex();

    gl_Position = pv * vec4(((-cam_right - cam_up) * bb[0].size) + bb[0].pos, 1.0); // bottom right 
    frag_color = bb[0].color;
    EmitVertex();

    gl_Position = pv * vec4(((cam_right + cam_up) * bb[0].size) + bb[0].pos, 1.0); // top left
    frag_color = bb[0].color;
    EmitVertex();

    gl_Position = pv * vec4(((-cam_right + cam_up) * bb[0].size) + bb[0].pos, 1.0); // top right
    frag_color = bb[0].color;
    EmitVertex();

    EndPrimitive();
}