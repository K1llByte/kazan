#version 450

// Input
layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;

// Uniform sampler
layout(binding = 1) uniform sampler2D tex_sampler;

// Output
layout(location = 0) out vec4 out_color;

//////////////////////////////////////////////////////////

void main()
{
    // out_color = texture(tex_sampler, frag_tex_coord);
    
    out_color = vec4(frag_color, 1.0);
}