#version 330

void main () {
    vec3 points[3] = vec3[3] (
        vec3(-0.5, -0.5, 0.0),
        vec3(0.5, -0.5, 0.0),
        vec3(0.0, 0.5, 0.0)
    );
    gl_Position = vec4(points[gl_VertexID], 1.0);
}