#version 450

layout(location = 0) in vec3 in_world_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_color;

layout(location = 0) out vec4 out_color;

#define M_PI 3.14159265358979323846

#define USE_BRANCHLESS_COMPUTE_LIGHT

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_LIGHTS 8

struct Light {
    int type;
    float intensity;
    float range;
    float spot_inner;
    float spot_outer;
    vec3 direction;
    vec3 color;
    vec3 position;
};

layout(set = 1, binding = 0) uniform Lights {
    int count;
    Light data[MAX_LIGHTS];
} lights;

#ifdef USE_BRANCHLESS_COMPUTE_LIGHT

vec3 compute_light(Light light, vec3 position, vec3 normal) {
    vec3 light_dir = mix(
        -light.direction,
        light.position - position,
        float(light.type != LIGHT_TYPE_DIRECTIONAL)
    );
    const float light_dist = length(light_dir);
    light_dir /= light_dist;

    const float attenuation = mix(
        1.0,
        clamp(1.0 - light_dist / light.range, 0.0, 1.0),
        float(light.type != LIGHT_TYPE_DIRECTIONAL)
    );

    const float spot_cos = dot(-light_dir, light.direction);
    const float cone_attenuation = mix(
        1.0,
        smoothstep(cos(light.spot_outer), cos(light.spot_inner), spot_cos),
        float(light.type == LIGHT_TYPE_SPOT)
    );

    const float n_dot_l = max(dot(normal, light_dir), 0.0);
    return light.color * light.intensity * n_dot_l * attenuation * cone_attenuation;
}

#else

vec3 compute_directional_light(Light light, vec3 normal) {
    const vec3 light_dir = -light.direction;
    const float n_dot_l = max(dot(normal, light_dir), 0.0);
    return light.color * light.intensity * n_dot_l;
}

vec3 compute_point_light(Light light, vec3 position, vec3 normal) {
    vec3 light_dir = light.position - position;
    const float light_dist = length(light_dir);
    light_dir /= light_dist;

    const float attenuation = clamp(1.0 - light_dist / light.range, 0.0, 1.0);
    const float n_dot_l = max(dot(normal, light_dir), 0.0);
    return light.color * light.intensity * n_dot_l * attenuation;
}

vec3 compute_spot_light(Light light, vec3 position, vec3 normal) {
    vec3 light_dir = light.position - position;
    const float light_dist = length(light_dir);
    light_dir /= light_dist;

    const float spot_cos = dot(-light_dir, normalize(light.direction));
    const float cone_attenuation = smoothstep(
        cos(light.spot_outer),
        cos(light.spot_inner),
        spot_cos
    );
    const float attenuation = clamp(1.0 - light_dist / light.range, 0.0, 1.0);

    const float n_dot_l = max(dot(normal, light_dir), 0.0);
    return light.color * light.intensity * n_dot_l * attenuation * cone_attenuation;
}
#endif

void main() {
    vec3 light_color = vec3(0);
#ifndef USE_BRANCHLESS_COMPUTE_LIGHT
    for(int i = 0; i < lights.count; ++i) {
        if(lights.data[i].type == LIGHT_TYPE_DIRECTIONAL) {
            light_color += compute_directional_light(lights.data[i], in_normal);
        }
        else if (lights.data[i].type == LIGHT_TYPE_POINT) {
            light_color += compute_point_light(lights.data[i], in_world_position, in_normal);
        }
        else if(lights.data[i].type == LIGHT_TYPE_SPOT) {
            light_color += compute_spot_light(lights.data[i], in_world_position, in_normal);
        }
    }

#else
    for(int i = 0; i < lights.count; ++i) {
        light_color += compute_light(lights.data[i], in_world_position, in_normal);
    }
#endif
    out_color = vec4(light_color * in_color, 1.0);
}

// float ndf_trowbridge_reitz_ggx(vec3 n, vec3 h, float a) {
//     const float a2 = a * a;
//     const float n_dot_h  = max(dot(n, h), 0.0);
//     const float denom  = n_dot_h * n_dot_h * (a2 - 1.0) + 1.0;
//     return a2 / (M_PI * denom * denom);
// }

// float g_schlick_ggx(vec3 n, vec3 v, float k) {
//     const float n_dot_v = max(dot(n, v), 0.0);
//     return n_dot_v / (n_dot_v * (1 - k) + k);
// }

// float g_smith(vec3 n, vec3 v, vec3 l, float k) {
//     return g_schlick_ggx(n, v, k) * g_schlick_ggx(n, l, k);
// }

// vec3 g_fresnel_schlick(vec3 h, vec3 v, vec3 f0) {
//     return f0 + (1.0 - f0) * pow(1.0 - max(dot(h, v), 0.0), 5.0);
// }