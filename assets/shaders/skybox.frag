#version 450

layout(location = 0) in vec2 in_ndc;

layout(set = 0, binding = 0) uniform Camera3D {
    float aspect_ratio;
    float fov_v;
    vec3 position;
    vec3 forward;
    vec3 up;
} camera;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(0,0,0,1);
    // out_color = vec4(camera.forward*0.5 + 0.5,1);
}