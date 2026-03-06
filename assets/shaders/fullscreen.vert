#version 450

layout(location = 0) out vec2 out_ndc;

const vec2 positions[] = vec2[](
    vec2(-1, -1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(1, 1)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0, 1.0);
    out_ndc = positions[gl_VertexIndex];
}