#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_position;
layout(location = 2) in vec3 frag_normal;

// Output
layout(location = 0) out vec4 out_color;

// Constants
const vec3 DIR_TO_LIGHT = normalize(vec3(1.0, 6.0, -10.0));
const float AMBIENT_LIGHT = 0.2;

//////////////////////////////////////////////////////////

void main()
{
    const vec3 norm_frag_normal = normalize(frag_normal);
    float light_intensity = max(dot(norm_frag_normal, DIR_TO_LIGHT), 0) + AMBIENT_LIGHT;
    out_color = vec4(frag_color * light_intensity, 1.0);
}