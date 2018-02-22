#version 450 core
in vec3 color;
in float fog;

out vec4 frag_color;

uniform vec3 fog_color = vec3(0.5);
uniform float alpha = 1.0;

void main() {
    frag_color = vec4(mix(color, fog_color, fog), alpha);
}