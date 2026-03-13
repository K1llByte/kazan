#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_color;

layout(set = 0, binding = 0) uniform Camera3D {
    float aspect_ratio;
    float fov_v;
    vec3 position;
    vec3 forward;
    vec3 up;
    mat4 proj_view;
} camera;

void main() {
    // gl_Position = vec4(in_position.xy, 0.0, 1.0);
    gl_Position = camera.proj_view * vec4(in_position, 1.0);
    out_color = in_color;
    out_normal = in_normal;
}