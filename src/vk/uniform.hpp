#pragma once

#include "boost/pfr/core.hpp"
#include "core/traits.hpp"
#include "math/types.hpp"
#include <type_traits>

//! Helper namespace for GLSL std140 layout compliant algebra types.
namespace kzn::glsl {

template<typename T, size_t Align>
    requires(Align >= sizeof(T))
class AlignedUniformType : public T {
public:
    using T::T;
    AlignedUniformType() = default;
    AlignedUniformType(const T& value)
        : T(value) {}
    AlignedUniformType(T&& value)
        : T(std::move(value)) {}

private:
    char m_padding[Align - sizeof(T)];
};

// Fundamental types
using Float = float;
using Double = double;
using Int = int;
using Uint = uint;
using Bool = bool;

// float vector types
using Vec2 = AlignedUniformType<Vec2, sizeof(Vec2)>;
using Vec3 = AlignedUniformType<Vec3, sizeof(Vec4)>;
using Vec4 = AlignedUniformType<Vec4, sizeof(Vec4)>;
// double vector types
using Vec2d = Vec2d;
using Vec3d = AlignedUniformType<Vec3d, sizeof(Vec4d)>;
using Vec4d = AlignedUniformType<Vec4d, sizeof(Vec4d)>;
// int vector types
using Vec2i = AlignedUniformType<Vec2i, sizeof(Vec2i)>;
using Vec3i = AlignedUniformType<Vec3i, sizeof(Vec4i)>;
using Vec4i = AlignedUniformType<Vec4i, sizeof(Vec4i)>;
// uint vector types
using Vec2u = AlignedUniformType<Vec2u, sizeof(Vec2u)>;
using Vec3u = AlignedUniformType<Vec3u, sizeof(Vec4u)>;
using Vec4u = AlignedUniformType<Vec4u, sizeof(Vec4u)>;
// bool vector types
using Vec2b = AlignedUniformType<Vec2b, sizeof(Vec2b)>;
using Vec3b = AlignedUniformType<Vec3b, sizeof(Vec4b)>;
using Vec4b = AlignedUniformType<Vec4b, sizeof(Vec4b)>;

// float matrix types
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
// double matrix types
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

//! If T is a std140 layout compliant type, provides the member constant value
//! equal to true. Otherwise value is false.
template<class T>
struct is_uniform
    : is_any_of<
          T,
          // clang-format off
          Float, Double, Int, Uint, Bool,
          Vec2, Vec3, Vec4,
          Vec2d, Vec3d, Vec4d,
          Vec2i, Vec3i, Vec4i,
          Vec2u, Vec3u, Vec4u,
          Vec2b, Vec3b, Vec4b,
          Mat2, Mat3, Mat4,
          Mat22, Mat23, Mat24,
          Mat32, Mat33, Mat34,
          Mat42, Mat43, Mat44,
          Mat2d, Mat3d, Mat4d,
          Mat22d, Mat23d, Mat24d,
          Mat32d, Mat33d, Mat34d,
          Mat42d, Mat43d, Mat44d
          // clang-format on
          > {};

template<class T>
constexpr bool is_uniform_v = is_uniform<T>::value;

//! Auxiliary function for UniformBlock concept requirements
template<typename T>
    requires std::is_default_constructible_v<T>
[[nodiscard]]
constexpr bool is_uniform_block() {
    bool has_non_uniform = false;
    boost::pfr::for_each_field(T{}, [&has_non_uniform](const auto& field) {
        if (!glsl::is_uniform_v<std::decay_t<decltype(field)>>) {
            has_non_uniform = true;
            return;
        }
    });
    return !has_non_uniform;
}

//! The concept UniformBlock<T> is satisfied if T is a struct that only contains
//! std140 layout compliant data members.
//! \b Example:
//! \code
//! struct FooBar {
//!     glsl::Float foo;
//!     glsl::Mat43 bar;
//! };
//! \endcode
//! \note A current limitation is that T needs to be default
//! constructible.
template<typename T>
concept UniformBlock = is_uniform_block<T>();

} // namespace kzn::glsl