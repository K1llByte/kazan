#version 450

#extension GL_ARB_shading_language_include : enable
#include "planet_config.glsl"

#define M_PI 3.14159265358979323846

layout(location = 0) in vec2 in_ndc;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform Camera3D {
    float aspect_ratio;
    float fov_v;
    vec3 position;
    vec3 forward;
    vec3 up;
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
    float pz = -1.0 / tan(fov_v * 0.5);

    return normalize(px * cam_right + py * cam_up + pz * camera.forward);
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

// Atmosphere density at a sample position.
float atmosphere_density_at_pos(vec3 sample_pos) {
    float inv_atmosphere_height = 1.0 / (ATMOSPHERE_RADIUS - EARTH_RADIUS);
    float height_above_surface =
        length(sample_pos - EARTH_POSITION) - EARTH_RADIUS;

    // 0 is at ground surface, 1 is at atmosphere surface.
    float normalized_height = height_above_surface * inv_atmosphere_height;

    return exp(-normalized_height * ATMOSPHERE_DENSITY) *
           (1.0 - normalized_height);
}

// Optical depth is the total accumulated atmospheric density along the light
// path
float optical_depth_at_pos(
    vec3 start_pos,
    vec3 ray_dir,
    float atmosphere_through_dist
) {
    float inv_samples = 1.0 / (float(OPTICAL_DEPTH_SAMPLES) - 1.0);
    float sample_dist = atmosphere_through_dist * inv_samples;
    vec3 step_dist = ray_dir * sample_dist;

    vec3 density_sample_pos = start_pos;
    float optical_depth = 0.0;

    for (int i = 0; i < OPTICAL_DEPTH_SAMPLES; ++i) {
        optical_depth +=
            atmosphere_density_at_pos(density_sample_pos) * sample_dist;
        density_sample_pos += step_dist;
    }

    return optical_depth;
}

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

// Rayleigh phase function (symmetric scattering)
// Nishita et al. 1993
float rayleigh_phase(float cos_theta) {
    return 0.75 * (1.0 + cos_theta * cos_theta);
}

// Mie phase function (Henyey–Greenstein phase function)
// https://en.wikipedia.org/wiki/Henyey%E2%80%93Greenstein_phase_function
float mie_phase(float cos_theta) {
    const float inv_4pi = 0.07957747154594767;
    const float g2 = MIE_HENYEY_GREENSTEIN_G * MIE_HENYEY_GREENSTEIN_G;
    const float denom = 1.0 + g2 - 2.0 * MIE_HENYEY_GREENSTEIN_G * cos_theta;
    return inv_4pi * (1.0 - g2) / (denom * sqrt(denom));
}

struct AtmosphereResult {
    vec3 in_scattered_light;
    vec3 transmittance;
};

AtmosphereResult atmosphere(
    vec3 start_pos,
    vec3 ray_dir,
    float atmosphere_through_dist
) {
    const vec3 rayleigh_coefficients = vec3(
        pow(RAYLEIGH_SCATTERING_BIAS / RAYLEIGH_SCATTERING_WAVELENGTHS.r, 4.0) *
            RAYLEIGH_SCATTERING_STRENGTH,
        pow(RAYLEIGH_SCATTERING_BIAS / RAYLEIGH_SCATTERING_WAVELENGTHS.g, 4.0) *
            RAYLEIGH_SCATTERING_STRENGTH,
        pow(RAYLEIGH_SCATTERING_BIAS / RAYLEIGH_SCATTERING_WAVELENGTHS.b, 4.0) *
            RAYLEIGH_SCATTERING_STRENGTH
    );

    const vec3 light_dir = LIGHT_DIRECTION;
    const float inv_samples = 1.0 / (float(IN_SCATTERING_SAMPLES) - 1.0);
    const float sample_dist = atmosphere_through_dist * inv_samples;
    const float cos_theta = dot(ray_dir, -light_dir);
    const float rayleigh_phase_value = rayleigh_phase(cos_theta);
    const float mie_phase_value = mie_phase(cos_theta);
    const vec3 step_dist = ray_dir * sample_dist;

    vec3 in_scatter_pos = start_pos;
    vec3 in_scattered_light = vec3(0.0);

    float accum_cam_optical_depth = 0.0;

    for (int i = 0; i < IN_SCATTERING_SAMPLES; i++) {
        const float light_length = ray_intersect_sphere(
            EARTH_POSITION, ATMOSPHERE_RADIUS, in_scatter_pos, -light_dir
        ).y;
        const float light_optical_depth = optical_depth_at_pos(in_scatter_pos, -light_dir, light_length);
        const float sample_density = atmosphere_density_at_pos(in_scatter_pos);
        const float cam_optical_depth_increment = sample_density * sample_dist;

        const vec3 transmittance =
            exp(-(rayleigh_coefficients + MIE_SCATTERING_STRENGTH) *
                (light_optical_depth + accum_cam_optical_depth));
        const vec3 scattering = rayleigh_coefficients * rayleigh_phase_value +
                          MIE_SCATTERING_STRENGTH * mie_phase_value;
        in_scattered_light += sample_density * transmittance * scattering *
                              LIGHT_COLOR * sample_dist;

        accum_cam_optical_depth += cam_optical_depth_increment;
        in_scatter_pos += step_dist;
    }

    AtmosphereResult result;
    result.transmittance =
        exp(-accum_cam_optical_depth *
            (rayleigh_coefficients + MIE_SCATTERING_STRENGTH));
    result.in_scattered_light = in_scattered_light;
    return result;
}

void main() {
    out_color = vec4(0, 0, 0, 1);

    const vec3 right = normalize(cross(camera.forward, vec3(0, -1, 0)));

    vec3 ray_dir = ray_direction(
        in_ndc, camera.fov_v, camera.aspect_ratio, camera.forward, right, camera.up
    );

    vec4 earth_color = out_color;
    vec2 earth_hit = ray_intersect_sphere(
        EARTH_POSITION, EARTH_RADIUS, camera.position, ray_dir
    );
    if (earth_hit.x > 0.0) {
        vec3 hit_pos = camera.position + ray_dir * earth_hit.x;
        vec3 normal = sphere_normal(hit_pos, EARTH_POSITION, EARTH_RADIUS);
        vec3 albedo = texture(earth_sampler, sphere_uv(hit_pos, EARTH_POSITION)).xyz;
        vec3 diffuse = albedo * LIGHT_COLOR * lambert_diffuse(LIGHT_DIRECTION, normal);
        vec3 ambient = albedo * LIGHT_COLOR * AMBIENT_STRENGTH;

        earth_color = vec4(diffuse + ambient, 1.0);
    }

#ifdef ENABLE_ATMOSPHERE
    vec2 atmosphere_hit = ray_intersect_sphere(
        EARTH_POSITION, ATMOSPHERE_RADIUS, camera.position, ray_dir
    );
    if (atmosphere_hit.y != 0.0) {
        if (earth_hit.x > 0.0) {
            atmosphere_hit.y = earth_hit.x - atmosphere_hit.x;
        }

        vec3 pos_in_atmosphere = camera.position + ray_dir * atmosphere_hit.x;
        // Scatter atmosphere light
        AtmosphereResult atmosphere_res =
            atmosphere(pos_in_atmosphere, ray_dir, atmosphere_hit.y);

        // Relative luminance of transmittance
        const float composited_alpha = 1.0 - dot(atmosphere_res.transmittance, vec3(0.2126, 0.7152, 0.0722));

        out_color = vec4(
            atmosphere_res.in_scattered_light + atmosphere_res.transmittance * earth_color.rgb,
            max(composited_alpha, earth_color.a)
        );
    }
#else
    out_color = earth_color;
#endif
}