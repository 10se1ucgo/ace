#version 330 core
in vec3 color;
in float diffuse;
in float fog;

out vec4 frag_color;

uniform vec3 fog_color;

void main() {
    frag_color = vec4(mix(color * (1 + 0.4 * diffuse), fog_color, fog), 1.0);
}