#pragma once

// #define GLM_FORCE_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace kzn {

// float
using Vec1 = glm::vec1;
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
// double
using Vec1d = glm::dvec1;
using Vec2d = glm::dvec2;
using Vec3d = glm::dvec3;
using Vec4d = glm::dvec4;
// int
using Vec1i = glm::ivec1;
using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;
// uint
using Vec1u = glm::uvec1;
using Vec2u = glm::uvec2;
using Vec3u = glm::uvec3;
using Vec4u = glm::uvec4;
// bool
using Vec1b = glm::bvec1;
using Vec2b = glm::bvec2;
using Vec3b = glm::bvec3;
using Vec4b = glm::bvec4;

using Quat = glm::quat;

// float
using Mat2 = glm::mat2;
using Mat3 = glm::mat3;
using Mat4 = glm::mat4;
using Mat22 = glm::mat2x2;
using Mat23 = glm::mat2x3;
using Mat24 = glm::mat2x4;
using Mat32 = glm::mat3x2;
using Mat33 = glm::mat3x3;
using Mat34 = glm::mat3x4;
using Mat42 = glm::mat4x2;
using Mat43 = glm::mat4x3;
using Mat44 = glm::mat4x4;
// double
using Mat2d = glm::dmat2;
using Mat3d = glm::dmat3;
using Mat4d = glm::dmat4;
using Mat22d = glm::dmat2x2;
using Mat23d = glm::dmat2x3;
using Mat24d = glm::dmat2x4;
using Mat32d = glm::dmat3x2;
using Mat33d = glm::dmat3x3;
using Mat34d = glm::dmat3x4;
using Mat42d = glm::dmat4x2;
using Mat43d = glm::dmat4x3;
using Mat44d = glm::dmat4x4;
// int
using Mat22i = glm::imat2x2;
using Mat23i = glm::imat2x3;
using Mat24i = glm::imat2x4;
using Mat32i = glm::imat3x2;
using Mat33i = glm::imat3x3;
using Mat34i = glm::imat3x4;
using Mat42i = glm::imat4x2;
using Mat43i = glm::imat4x3;
using Mat44i = glm::imat4x4;
// uint
using Mat22u = glm::umat2x2;
using Mat23u = glm::umat2x3;
using Mat24u = glm::umat2x4;
using Mat32u = glm::umat3x2;
using Mat33u = glm::umat3x3;
using Mat34u = glm::umat3x4;
using Mat42u = glm::umat4x2;
using Mat43u = glm::umat4x3;
using Mat44u = glm::umat4x4;

} // namespace kzn