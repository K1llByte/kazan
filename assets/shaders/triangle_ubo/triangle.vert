#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 out_color;

layout(binding = 0) uniform Pvm {
    mat4 model;
    mat4 view;
    mat4 proj;
} pvm;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0) * pvm.proj * pvm.view  * pvm.model;
    out_color = in_color;
}