#version 450

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_tex;

// // Uniform buffer block
// layout(set = 0, binding = 0) uniform PVM {
//     mat4 model;
//     mat4 view;
//     mat4 projection;
// } pvm;

// Push constants block
layout(push_constant) uniform PVM
{
    mat4 proj_view;
    mat4 model;
} pvm;

// Output
layout(location = 0) out vec3 out_color;
layout(location = 1) out vec3 out_frag_pos;
layout(location = 2) out vec3 out_normal;

//////////////////////////////////////////////////////////

void main()
{
    // gl_Position = pvm.projection * pvm.view * pvm.model * vec4(v_position, 1.0f);
    gl_Position = pvm.proj_view * pvm.model * vec4(v_position, 1.0f);
    out_frag_pos = vec3(pvm.model * vec4(v_position, 1.0));
    out_normal = mat3(transpose(inverse(pvm.model))) * normalize(v_normal);
    out_color = v_color;
}