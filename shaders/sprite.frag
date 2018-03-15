#version 330 core
in vec2 f_tex;
in vec4 f_tint;

out vec4 color;

uniform sampler2D sprite_tex;

void main() {
    color = texture(sprite_tex, f_tex.st) * f_tint;
}