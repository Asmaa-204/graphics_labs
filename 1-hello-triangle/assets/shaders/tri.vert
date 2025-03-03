#version 330
out vec4 vertexColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform float time;

void main () {
    gl_Position = vec4(position, 1.0);
    vertexColor = color;
}