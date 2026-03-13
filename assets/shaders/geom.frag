#version 450

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec4 out_color;


void main() {
    const vec3 diffuse = clamp(dot(in_normal, vec3(1,1,1)), 0.0, 1.0) * in_color; 
    out_color = vec4(diffuse, 1.0);
}