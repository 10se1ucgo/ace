#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in Billboard {
    vec3 pos;
    vec3 color;
    float size;
} bb[];

out vec3 frag_color;

uniform vec3 cam_left, cam_up;
uniform mat4 pv;


void main() {
    vec3 color = bb[0].color;
    float size = bb[0].size;
    
    mat4 m = mat4(1.0);
    m[3].xyz = bb[0].pos;

    mat4 mvp = pv * m;

    gl_Position = mvp * vec4(-(cam_left + cam_up) * size, 1.0);
    frag_color = color;
    EmitVertex();
    gl_Position = mvp * vec4(-(cam_left - cam_up) * size, 1.0);
    frag_color = color;
    EmitVertex();
    gl_Position = mvp * vec4((cam_left - cam_up) * size, 1.0);
    frag_color = color;
    EmitVertex();
    gl_Position = mvp * vec4((cam_left + cam_up) * size, 1.0);
    frag_color = color;
    EmitVertex();

    EndPrimitive();
}