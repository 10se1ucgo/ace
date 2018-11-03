#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in TextGlyph {
    vec2 position;
    vec2 size;
    vec4 tex_coords; // tl/br
    vec3 color;
} glyph[];

out vec2 f_tex_coords;
out vec3 f_color;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(glyph[0].position, 0.0, 1.0);
    f_color = glyph[0].color;
    f_tex_coords = glyph[0].tex_coords.xy; // top left
    EmitVertex();

    gl_Position = mvp * vec4(glyph[0].position + vec2(glyph[0].size.x, 0), 0.0, 1.0);
    f_color = glyph[0].color;
    f_tex_coords = glyph[0].tex_coords.zy; // top right
    EmitVertex();

    gl_Position = mvp * vec4(glyph[0].position + vec2(0, glyph[0].size.y), 0.0, 1.0);
    f_color = glyph[0].color;
    f_tex_coords = glyph[0].tex_coords.xw; // bottom left
    EmitVertex();

    gl_Position = mvp * vec4(glyph[0].position + glyph[0].size, 0.0, 1.0);
    f_color = glyph[0].color;
    f_tex_coords = glyph[0].tex_coords.zw; // bottom right
    EmitVertex();

    EndPrimitive();
}