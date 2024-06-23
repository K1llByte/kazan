#pragma once

#include "math/types.hpp"
#include <cmath>
#include <type_traits>
namespace kzn {

//! Converts a Vec3 from cartesian to spherical coordinates.
//! \note Coordinate system is according to OpenGL standard.
[[nodiscard]]
constexpr Vec3 cartesian_to_spherical(Vec3 vec) {
    const auto x = vec.x;
    const auto y = vec.y;
    const auto z = vec.z;

    const auto abs = std::sqrt(x * x + y * y + z * z);
    return Vec3{abs, std::atan2(x, z), std::acos(y / abs)};
}

//! Converts a Vec3 from spherical to cartesian coordinates.
//! \note Coordinate system is according to OpenGL standard.
[[nodiscard]]
constexpr Vec3 spherical_to_cartesian(Vec3 vec) {
    const float p = vec.x;
    const float theta = vec.y;
    const float phi = vec.z;

    return Vec3{
        std::sin(theta) * std::sin(phi) * p,
        std::cos(phi) * p,
        std::sin(phi) * p * std::cos(theta)
    };
}

//! Converts a Vec3 from cylindrical to cartesian coordinates.
//! \note Coordinate system is according to OpenGL standard.
[[nodiscard]]
constexpr Vec3 cylindrical_to_cartesian(Vec3 vec) {
    const float radius = vec.x;
    const float theta = vec.z;

    return Vec3{radius * std::sin(theta), vec.y, radius * std::cos(theta)};
}

//! Converts a Vec3 from cartesian to cylindrical coordinates.
//! \note Coordinate system is according to OpenGL standard.
[[nodiscard]]
constexpr Vec3 cartesian_to_cylindrical(Vec3 vec) {
    const float x = vec.x;
    const float y = vec.y;
    const float z = vec.z;

    return Vec3{std::sqrt(x * x + z * z), y, std::atan2(x, z)};
}

} // namespace kzn