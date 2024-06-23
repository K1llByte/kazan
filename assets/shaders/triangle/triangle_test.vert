#version 450

#extension GL_EXT_scalar_block_layout : enable

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) out vec3 vert_color;

layout(set = 0, binding = 0, scalar) uniform Transform {
    mat3 mat;
} transform;

// Triangle
const vec2 position[] = vec2[] (
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

// Triangle colors
const vec3 colors[] = vec3[](
    vec3(0.984, 0.286, 0.203),
    vec3(0.556, 0.752, 0.486),
    vec3(0.513, 0.647, 0.596)
);


void main()
{
    const vec3 pos = transform.mat * vec3(position[gl_VertexIndex], 1);
    gl_Position = vec4(pos.xy, 0.0, 1.0);

    vert_color = colors[gl_VertexIndex];
}