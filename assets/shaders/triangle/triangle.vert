#version 450

layout(location = 0) out vec3 vert_color;

// Triangle
const vec2 position[3] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// Triangle colors
// RGB
// const vec3 colors[3] = vec3[](
//     vec3(1.0, 0.0, 0.0),
//     vec3(0.0, 1.0, 0.0),
//     vec3(0.0, 0.0, 1.0)
// );
// Gruvbox RGB
const vec3 colors[3] = vec3[](
    vec3(0.984, 0.286, 0.203),
    vec3(0.556, 0.752, 0.486),
    vec3(0.513, 0.647, 0.596)
);


void main()
{
    gl_Position = vec4(position[gl_VertexIndex], 0.0, 1.0);
    vert_color = colors[gl_VertexIndex];
}