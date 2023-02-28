#version 450

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_tex;

// Push constants block
layout(push_constant) uniform PVM {
    mat4 proj_view;
    mat4 model;
} pvm;

// Descriptor sets block
// layout(set = 0, binding = 0) uniform Tmp {
//     vec3 bias1;
//     vec3 bias2;
// } tmp;

// Output
layout(location = 0) out vec3 out_color;
layout(location = 1) out vec3 out_frag_pos;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec2 out_tex_coords;

//////////////////////////////////////////////////////////

void main()
{
    gl_Position = pvm.proj_view * pvm.model * vec4(v_position, 1.0f);

    // vec3 transformed_normal = normalize(mat3(pvm.model) * v_normal);
    // float light_intensity = max(dot(v_normal, DIR_TO_LIGHT), 0) + AMBIENT_LIGHT;
    // out_color = normalize(v_normal * light_intensity);

    out_frag_pos = vec3(pvm.model * vec4(v_position, 1.0));
    out_normal = mat3(transpose(inverse(pvm.model))) * normalize(v_normal);
    // out_color = tmp.bias2.xyz;
    out_tex_coords = v_tex;
}