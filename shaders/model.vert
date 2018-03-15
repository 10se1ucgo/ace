#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 kv6norm;

out vec3 color;
out float diffuse;
out float ambient;
out float fog;

uniform mat4 mvp;
uniform vec3 light_pos;
uniform mat3 normal_matrix;

uniform vec3 filter_color = vec3(0.0);
uniform vec3 replacement_color = vec3(39, 153, 41) / 255.0;

void main() {
    gl_Position = mvp * vec4(pos, 1.0);

    vec3 norm = normalize(normal_matrix * ((kv6norm *.75) + (normal * .25)));
    diffuse = max(dot(norm, light_pos), 0.0);
    ambient = 0.5;

    color = vertex_color == filter_color ? replacement_color : vertex_color;
    fog = 1.0 - clamp((128 - length(gl_Position.xyz)) / 64, 0.0, 1.0);
}