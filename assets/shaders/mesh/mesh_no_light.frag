#version 450

// Input
layout(location = 0) in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

//////////////////////////////////////////////////////////

void main()
{
    out_color = vec4(frag_color, 1.0);
}