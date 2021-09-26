#version 450

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_tex;

// Uniform buffer block
layout(set = 0, binding = 0) uniform PVM {
    mat4 model;
    mat4 view;
    mat4 projection;
} pvm;

// // Push constants block
// layout(push_constant) uniform constants
// {
//     vec4 data;
//     mat4 pvm;
// } push;

// Output
layout(location = 0) out vec3 out_color;

const vec3 DIR_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT_LIGHT = 0.02;

//////////////////////////////////////////////////////////

void main()
{
    // gl_Position = push.pvm * vec4(v_position, 1.0f);
    gl_Position = pvm.projection * pvm.view * pvm.model * vec4(v_position, 1.0f);

    vec3 transformed_normal = normalize(mat3(pvm.model) * v_normal);
    float light_intensity = max(dot(v_normal, DIR_TO_LIGHT), 0) + AMBIENT_LIGHT;
    out_color = v_color * light_intensity;
}