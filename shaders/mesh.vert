#version 450

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;

// Uniform buffer block
layout(set = 0, binding = 0) uniform PVM {
	mat4 model;
	mat4 view;
	mat4 projection;
} pvm;

// Output
layout(location = 0) out vec3 out_color;

// Push constants block
// layout(push_constant) uniform constants
// {
//     vec4 data;
//     mat4 pvm;
// } push;

void main()
{
    // gl_Position = push.pvm * vec4(v_position, 1.0f);
    gl_Position = pvm.projection * pvm.view * pvm.model * vec4(v_position, 1.0f);
    out_color = v_color;
}