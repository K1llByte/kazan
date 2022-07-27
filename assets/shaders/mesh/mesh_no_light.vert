#version 450

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_tex;

// Push constants block
layout(push_constant) uniform PVM
{
    mat4 proj_view;
    mat4 model;
} pvm;

// Descriptor sets block
layout(set = 0, binding = 0) uniform Tmp {
    vec3 bias;  
} tmp;

// Output
layout(location = 0) out vec3 out_color;

//////////////////////////////////////////////////////////

void main()
{
    // gl_Position = push.pvm * vec4(v_position, 1.0f);
    gl_Position = pvm.proj_view * pvm.model * vec4(v_position, 1.0f);
    out_color = v_color * tmp.bias;
}