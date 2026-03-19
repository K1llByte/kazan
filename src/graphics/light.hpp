#pragma once

#include "core/assert.hpp"
#include "glm/geometric.hpp"
#include "math/types.hpp"
#include "vk/uniform.hpp"

#define KZN_LIGHT_TYPE_DIRECTIONAL 0
#define KZN_LIGHT_TYPE_POINT 1
#define KZN_LIGHT_TYPE_SPOT 2
#define KZN_MAX_LIGHTS 8

namespace kzn {

struct Light {
    glsl::Int type;
    glsl::Float intensity;
    glsl::Float range;
    glsl::Float spot_inner;
    glsl::Float spot_outer;
    glsl::Vec3 direction;
    glsl::Vec3 color;
    glsl::Vec3 position;
};

struct Lights {
    glsl::Int count;
    // NOTE: C style array does not work here due to problem with boost::pfr.
    std::array<Light, KZN_MAX_LIGHTS> data;
};

constexpr Light directional_light(
    float intensity,
    Vec3 direction,
    Vec3 color
) {
    KZN_ASSERT_MSG(
        glm::epsilonEqual(glm::length(direction), 1.0f, 1e-6f),
        "Light direction is not normalized"
    );
    return Light {
        .type = KZN_LIGHT_TYPE_DIRECTIONAL,
        .intensity = intensity,
        .direction = direction,
        .color = color,
    };
}

constexpr Light point_light(
    float intensity,
    Vec3 position,
    float range,
    Vec3 color
) {
    return Light {
        .type = KZN_LIGHT_TYPE_POINT,
        .intensity = intensity,
        .range = range,
        .color = color,
        .position = position,
    };
}

constexpr Light spot_light(
    float intensity,
    Vec3 position,
    Vec3 direction,
    float range,
    float spot_inner,
    float spot_outer,
    Vec3 color
) {
    KZN_ASSERT_MSG(
        glm::epsilonEqual(glm::length(direction), 1.0f, 1e-6f),
        "Light direction is not normalized"
    );
    
    return Light {
        .type = KZN_LIGHT_TYPE_SPOT,
        .intensity = intensity,
        .range = range,
        .spot_inner = spot_inner,
        .spot_outer = spot_outer,
        .direction = direction,
        .color = color,
        .position = position,
    };
}

} // namespace kzn