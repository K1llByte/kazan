#version 450

#extension GL_ARB_shading_language_include : enable
#include "planet_config.glsl"

#define M_PI 3.14159265358979323846

layout(location = 0) in vec2 in_ndc;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform Camera {
    vec2 position;
    float zoom;
    float aspect_ratio;
    float rotation;
} camera;
layout(set = 1, binding = 0) uniform sampler2D earth_sampler;

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

// // Atmosphere density at a sample position.
// float atmosphere_density_at_pos(vec3 sample_pos) {
//     float inv_atmosphere_height = 1.0 / (ATMOSPHERE_RADIUS - EARTH_RADIUS);
//     float height_above_surface =
//         length(sample_pos - EARTH_POSITION) - EARTH_RADIUS;

//     // 0 is at ground surface, 1 is at atmosphere surface.
//     float normalized_height = height_above_surface * inv_atmosphere_height;

//     return exp(-normalized_height * ATMOSPHERE_DENSITY) *
//            (1.0 - normalized_height);
// }

// // Optical depth is the total accumulated atmospheric density along the light
// // path
// float optical_depth_at_pos(
//     vec3 start_pos,
//     vec3 ray_dir,
//     float atmosphere_through_dist
// ) {
//     float inv_samples = 1.0 / (float(OPTICAL_DEPTH_SAMPLES) - 1.0);
//     float sample_dist = atmosphere_through_dist * inv_samples;
//     vec3 step_dist = ray_dir * sample_dist;

//     vec3 density_sample_pos = start_pos;
//     float optical_depth = 0.0;

//     for (int i = 0; i < OPTICAL_DEPTH_SAMPLES; ++i) {
//         optical_depth +=
//             atmosphere_density_at_pos(density_sample_pos) * sample_dist;
//         density_sample_pos += step_dist;
//     }

//     return optical_depth;
// }

vec3 sphere_normal(vec3 hit_pos, vec3 sphere_position, float sphere_radius) {
    return (hit_pos - sphere_position) * (1.0 / sphere_radius);
}

vec2 sphere_uv(vec3 hit_pos, vec3 sphere_position) {
    vec3 n = normalize(hit_pos - sphere_position);

    float u = atan(n.z, n.x) / (2.0 * M_PI) + 0.5;
    float v = 0.5 - asin(n.y) * (1.0 / M_PI);

    return vec2(u, v);
}

float lambert_diffuse(vec3 light_dir, vec3 normal) {
    return clamp(dot(normal, -light_dir), 0.0, 1.0);
}

void main() {
    vec3 right = normalize(cross(CAMERA_FORWARD, vec3(0, -1, 0)));
    vec3 up = normalize(cross(right, CAMERA_FORWARD));
    vec3 ray_dir = ray_direction(
        in_ndc, CAMERA_FOV, camera.aspect_ratio, CAMERA_FORWARD, right, up
    );

    vec2 hit = ray_intersect_sphere(
        EARTH_POSITION, EARTH_RADIUS, CAMERA_POSITION, ray_dir
    );
    if (hit.x > 0.0) {
        vec3 hit_pos = CAMERA_POSITION + ray_dir * hit.x;
        vec3 normal = sphere_normal(hit_pos, EARTH_POSITION, EARTH_RADIUS);
        vec4 albedo =
            texture(earth_sampler, sphere_uv(hit_pos, EARTH_POSITION));

        // out_color = vec4(1 - (normal * 0.5 + 0.5), 1);
        out_color = albedo;
    }
    else {
        // out_color = vec4(in_color, 1);
        out_color = vec4(0, 0, 0, 1);
    }
}