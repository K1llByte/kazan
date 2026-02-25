#version 450

#extension GL_ARB_shading_language_include : enable
#include "planet_config.glsl"

// layout(set = 0, binding = 0) uniform Camera {
//     vec2 position;
//     float zoom;
//     float aspect_ratio;
//     float rotation;
// }
// camera;

layout(location = 0) in vec2 in_ndc;

layout(location = 0) out vec4 out_color;

vec3 ray_direction(
    vec2 ndc,
    float fov_v,
    float aspect_ratio,
    vec3 cam_forward,
    vec3 cam_right,
    vec3 cam_up
) {
    float px = ndc.x * aspect_ratio;
    float py = ndc.y;
    float pz = 1.0 / tan(fov_v * 0.5);

    return normalize(px * cam_right + py * cam_up + pz * cam_forward);
}

// Branchless sphere ray intersection. Assumes ray_direction is normalized.
vec2 ray_intersect_sphere(
    vec3 sphere_position,
    float sphere_radius,
    vec3 ray_origin,
    vec3 ray_direction
) {
    vec3 distance = ray_origin - sphere_position;

    float b = 2.0 * dot(distance, ray_direction);
    float c = dot(distance, distance) - sphere_radius * sphere_radius;

    float discriminant = b * b - 4.0 * c;

    // Compute s assuming discriminant > 0
    // Even if s / near_t / far_t become NaN,
    // max(NaN, 0.0) will resolve to 0.0
    float s = sqrt(discriminant);

    float near_t = (-b - s) * 0.5;
    float far_t = (-b + s) * 0.5;

    // Valid if (discriminant > 0) and (far_t >= 0)
    float valid = step(0.0, discriminant) * step(0.0, far_t);

    float sphere_near = max(near_t, 0.0);
    float sphere_len = max(far_t - sphere_near, 0.0);

    return vec2(sphere_near, sphere_len) * valid;
}

void main() {
    vec3 right = normalize(cross(CAMERA_FORWARD, vec3(0, -1, 0)));
    vec3 up = normalize(cross(right, CAMERA_FORWARD));
    vec3 ray_dir = ray_direction(
        in_ndc, CAMERA_FOV, CAMERA_ASPECT_RATIO, CAMERA_FORWARD, right, up
    );

    vec2 hit = ray_intersect_sphere(
        EARTH_POSITION, EARTH_RADIUS, CAMERA_POSITION, ray_dir
    );
    if (hit.x > 0.0) {
        out_color = vec4(1, 0, 0, 1);
    }
    else {
        // out_color = vec4(in_color, 1);
        out_color = vec4(0, 0, 0, 1);
    }
}