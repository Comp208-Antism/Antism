#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;

uniform mat4 pr_matrix = mat4(1.0f);
uniform mat4 vw_matrix = mat4(1.0f);
uniform mat4 ml_matrix = mat4(1.0f);

out DATA {
    vec3 position;
    vec4 color;
    vec2 texCoords;
} vs_out;

void main() {
    gl_Position = pr_matrix * vw_matrix * ml_matrix * position;

    vs_out.position = vec3(ml_matrix * position);
    vs_out.color = color;
    vs_out.texCoords = vec2(texCoords.x, 1.0f - texCoords.y);
}