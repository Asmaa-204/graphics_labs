#version 330
out vec4 vertexColor;

void main () {
    const vec3 points[3] = vec3[3] (
        vec3(-0.5, -0.5, 0.0),
        vec3(0.5, -0.5, 0.0),
        vec3(0.0, 0.5, 0.0)
    );

    vec4 vertexColors[3] = vec4[3] (
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0),
        vec4(0.0, 0.0, 1.0, 1.0)
    );

    gl_Position = vec4(points[gl_VertexID], 1.0);
    vertexColor = vertexColors[gl_VertexID];
}