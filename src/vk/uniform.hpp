#pragma once

// #include <boost/pfr.hpp>

#include "math/types.hpp"

//! Helper namespace for GLSL std140 layout compliant algebra types.
namespace kzn::glsl {
template<class T, class... U>
inline constexpr bool is_any_v = (std::is_same_v<T, U> || ...);

// FIXME:
// - Can't assign glsl::vec3 v = glsl::vec3{1,2,3};
// - Can't use brace-enclosed initializer list glsl::vec3 v{1,2,3};
template<typename T, size_t Align>
    requires(Align >= sizeof(T))
class Uniform : public T {
public:
    using T::T;
    Uniform() = default;
    Uniform(const T& value)
        : T(value) {}
    Uniform(T&& value)
        : T(std::move(value)) {}
    // operator T&() const { return *this; }

private:
    char padding[Align - sizeof(T)];
};

// Basic types
using Float = float;
using Double = double;
using Int = int;
using Uint = uint;
using Bool = bool;

// float
using Vec2 = Uniform<Vec2, sizeof(Vec2)>;
using Vec3 = Uniform<Vec3, sizeof(Vec4)>;
using Vec4 = Uniform<Vec4, sizeof(Vec4)>;
// double
using Vec2d = Uniform<Vec2d, sizeof(Vec2d)>;
using Vec3d = Uniform<Vec3d, sizeof(Vec4d)>;
using Vec4d = Uniform<Vec4d, sizeof(Vec4d)>;
// int
using Vec2i = Uniform<Vec2i, sizeof(Vec2i)>;
using Vec3i = Uniform<Vec3i, sizeof(Vec4i)>;
using Vec4i = Uniform<Vec4i, sizeof(Vec4i)>;
// uint
using Vec2u = Uniform<Vec2u, sizeof(Vec2u)>;
using Vec3u = Uniform<Vec3u, sizeof(Vec4u)>;
using Vec4u = Uniform<Vec4u, sizeof(Vec4u)>;
// bool
using Vec2b = Uniform<Vec2b, sizeof(Vec2b)>;
using Vec3b = Uniform<Vec3b, sizeof(Vec4b)>;
using Vec4b = Uniform<Vec4b, sizeof(Vec4b)>;

// glm::mat<length_t C, length_t R, typename T>

// float
using Mat2 = Mat2;
using Mat3 = Mat3;
using Mat4 = Mat4;
using Mat22 = Mat22;
using Mat23 = Mat23;
using Mat24 = Mat24;
using Mat32 = Mat32;
using Mat33 = Mat33;
using Mat34 = Mat34;
using Mat42 = Mat42;
using Mat43 = Mat43;
using Mat44 = Mat44;
// double
using Mat2d = Mat2d;
using Mat3d = Mat3d;
using Mat4d = Mat4d;
using Mat22d = Mat22d;
using Mat23d = Mat23d;
using Mat24d = Mat24d;
using Mat32d = Mat32d;
using Mat33d = Mat33d;
using Mat34d = Mat34d;
using Mat42d = Mat42d;
using Mat43d = Mat43d;
using Mat44d = Mat44d;
// TODO: Finish this for all needed glsl primitive types

// template<typename T>
// constexpr bool is_uniform()
//     requires(std::is_default_constructible_v<T>)
// {
//     bool res = true;
//     boost::pfr::for_each_field(
//         std::forward<T>(T{}),
//         [&res]<typename Field>(Field&) {
//             res = is_any_v<
//                 Field,
//                 glsl::Bool,
//                 glsl::Int,
//                 glsl::Uint,
//                 glsl::Float,
//                 glsl::Double,
//                 glsl::bvec2,
//                 glsl::bvec3,
//                 glsl::bvec4,
//                 glsl::ivec2,
//                 glsl::ivec3,
//                 glsl::ivec4,
//                 glsl::uvec2,
//                 glsl::uvec3,
//                 glsl::uvec4,
//                 glsl::vec2,
//                 glsl::vec3,
//                 glsl::vec4,
//                 glsl::dvec2,
//                 glsl::dvec3,
//                 glsl::dvec4,
//                 glsl::mat2,
//                 glsl::mat3,
//                 glsl::mat4,
//                 glsl::mat2x3,
//                 glsl::mat2x4,
//                 glsl::mat3x2,
//                 glsl::mat3x4,
//                 glsl::mat4x2,
//                 glsl::mat4x3,
//                 glsl::dmat2,
//                 glsl::dmat3,
//                 glsl::dmat4,
//                 glsl::dmat2x3,
//                 glsl::dmat2x4,
//                 glsl::dmat3x2,
//                 glsl::dmat3x4,
//                 glsl::dmat4x2,
//                 glsl::dmat4x3>;
//         }
//     );
//     return res;
// }
} // namespace kzn::glsl