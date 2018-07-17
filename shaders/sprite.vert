#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 tint;
layout (location = 2) in vec4 region; // xy == bottom left, zw == top right
layout (location = 3) in mat3 model;
// 3 = col 0, 4 = col 1, 5 = col 2

out vec2 f_tex;
out vec4 f_tint;

uniform mat4 projection;
uniform vec2 dims;

// const vec2 tcoords[4] = vec2[](
//     vec2(0.0, 0.0),
//     vec2(0.0, 1.0),
//     vec2(1.0, 0.0),
//     vec2(1.0, 1.0)
// );

void main() {
    vec2 tcoords[4] = vec2[](
        region.xy,
        region.xw,
        region.zy,
        region.zw
    );

    gl_Position = (projection * mat4(model)) * vec4(pos, 1.0, 1.0);
    // gl_Position.z = level;

    f_tex = tcoords[gl_VertexID];
    f_tint = tint;
}