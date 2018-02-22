#version 450 core
in vec3 color;
in float diffuse;
in float ambient;
in float fog;

out vec4 frag_color;

uniform vec3 fog_color;

void main() {
    frag_color = vec4(mix(color * clamp(diffuse + ambient, 0.4, 1.3), fog_color, fog), 1.0);
}