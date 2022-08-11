#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_position;
layout(location = 2) in vec3 frag_normal;
layout(location = 3) in vec2 frag_tex_coords;

// Output
layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 1) uniform sampler2D tex_sampler;

// Constants
const vec3 DIR_TO_LIGHT = normalize(vec3(0.0, 6.0, -10.0));
const float AMBIENT_LIGHT = 0.2;

//////////////////////////////////////////////////////////

void main()
{
    const vec3 norm_frag_normal = normalize(frag_normal);
    float light_intensity = max(dot(norm_frag_normal, DIR_TO_LIGHT), 0) + AMBIENT_LIGHT;
    vec3 real_frag_color = texture(tex_sampler, frag_tex_coords).xyz;
    out_color = vec4(real_frag_color * light_intensity, 1.0);
}