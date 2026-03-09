#version 450

layout(location = 0) in vec2 in_ndc;

layout(set = 0, binding = 0) uniform Camera3D {
    float aspect_ratio;
    float fov_v;
    vec3 position;
    vec3 forward;
    vec3 up;
} cam;

layout(set = 1, binding = 0) uniform samplerCube in_skybox;

layout(location = 0) out vec4 out_color;

void main() {
    const vec3 right = normalize(cross(cam.forward, cam.up));
    const float tan_half_fov = tan(cam.fov_v * 0.5);
    const vec3 ray_dir = normalize(
        cam.forward +
        in_ndc.x * cam.aspect_ratio * tan_half_fov * right +
        in_ndc.y * tan_half_fov * cam.up
    );

    // TODO: Sample cubemap
    // out_color = vec4((ray_dir * 0.5 + 0.5) * 0.02, 1);
    out_color = texture(in_skybox, ray_dir);
}