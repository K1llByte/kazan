#ifndef KZN_VK_UNIFORM_HPP
#define KZN_VK_UNIFORM_HPP

#include <glm/glm.hpp>
#include <boost/pfr.hpp>

namespace kzn::glsl {
    template< class T, class ...U>
    inline constexpr bool is_any_v = (std::is_same_v<T, U> || ...);

    // FIXME:
    // - Can't assign glsl::vec3 v = glsl::vec3{1,2,3};
    // - Can't use brace-enclosed initializer list glsl::vec3 v{1,2,3};
    template<typename T, size_t Align>
        requires (Align >= sizeof(T))
    class Uniform: public T {
        public:
        Uniform() = default;
        Uniform(const T& value) : T(value) {}
        Uniform(T&& value) : T(std::move(value)) {}
        // operator T&() const { return *this; }

        private:
        char padding[Align-sizeof(T)];
    };

    // Basic types
    using Bool = bool;
    using Int = int;
    using Uint = uint;
    using Float = float;
    using Double = double;
    // bool vec
    using bvec2 = Uniform<glm::bvec2, sizeof(glm::bvec2)>;
    using bvec3 = Uniform<glm::bvec3, sizeof(glm::bvec4)>;
    using bvec4 = Uniform<glm::bvec4, sizeof(glm::bvec4)>;
    // int vec
    using ivec2 = Uniform<glm::ivec2, sizeof(glm::ivec2)>;
    using ivec3 = Uniform<glm::ivec3, sizeof(glm::ivec4)>;
    using ivec4 = Uniform<glm::ivec4, sizeof(glm::ivec4)>;
    // uint vec
    using uvec2 = Uniform<glm::uvec2, sizeof(glm::uvec2)>;
    using uvec3 = Uniform<glm::uvec3, sizeof(glm::uvec4)>;
    using uvec4 = Uniform<glm::uvec4, sizeof(glm::uvec4)>;
    // float vec
    using vec2 = Uniform<glm::vec2, sizeof(glm::vec2)>;
    using vec3 = Uniform<glm::vec3, sizeof(glm::vec4)>;
    using vec4 = Uniform<glm::vec4, sizeof(glm::vec4)>;
    // double vec
    using dvec2 = Uniform<glm::dvec2, sizeof(glm::dvec2)>;
    using dvec3 = Uniform<glm::dvec3, sizeof(glm::dvec4)>;
    using dvec4 = Uniform<glm::dvec4, sizeof(glm::dvec4)>;
    // Float Matricies
    using mat2 = glm::mat2;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    // Matricies 2xN
    using mat2x3 = glm::mat2x3;
    using mat2x4 = glm::mat2x4;
    // Matricies 3xN
    using mat3x2 = glm::mat3x2;
    using mat3x4 = glm::mat3x4;
    // Matricies 4xN
    using mat4x2 = glm::mat4x2;
    using mat4x3 = glm::mat4x3;
    // Double Matricies
    using dmat2 = glm::dmat2;
    using dmat3 = glm::dmat3;
    using dmat4 = glm::dmat4;
    // Matricies 2xN
    using dmat2x3 = glm::dmat2x3;
    using dmat2x4 = glm::dmat2x4;
    // Matricies 3xN
    using dmat3x2 = glm::dmat3x2;
    using dmat3x4 = glm::dmat3x4;
    // Matricies 4xN
    using dmat4x2 = glm::dmat4x2;
    using dmat4x3 = glm::dmat4x3;


    template<typename T>
    constexpr bool is_uniform()
        requires (std::is_default_constructible_v<T>)
    {
        bool res = true;
        boost::pfr::for_each_field(std::forward<T>(T{}), [&res]<typename Field>(Field&) {
            res = is_any_v<
                Field,
                glsl::Bool,
                glsl::Int,
                glsl::Uint,
                glsl::Float,
                glsl::Double,
                glsl::bvec2, glsl::bvec3, glsl::bvec4,
                glsl::ivec2, glsl::ivec3, glsl::ivec4,
                glsl::uvec2, glsl::uvec3, glsl::uvec4,
                glsl::vec2,  glsl::vec3,  glsl::vec4,
                glsl::dvec2, glsl::dvec3, glsl::dvec4,
                glsl::mat2,    glsl::mat3,   glsl::mat4,
                glsl::mat2x3,  glsl::mat2x4,
                glsl::mat3x2,  glsl::mat3x4,
                glsl::mat4x2,  glsl::mat4x3,
                glsl::dmat2,   glsl::dmat3,  glsl::dmat4,
                glsl::dmat2x3, glsl::dmat2x4,
                glsl::dmat3x2, glsl::dmat3x4,
                glsl::dmat4x2, glsl::dmat4x3>;
            });
        return res;
    }
} // namespace kzn::glsl;

#endif