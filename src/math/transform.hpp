#pragma once

#include "math/types.hpp"

#include <glm/fwd.hpp>

namespace kzn {

struct Transform2DComponent {
    Vec2 position = {0.f, 0.f};
    float depth = 0.f;
    float rotation = 0.f;
    Vec2 scale = {1.f, 1.f};

    [[nodiscard]]
    constexpr Mat4 matrix() const {
        // NOTE: Initialization of glm matrices is done in column-major.
        // clang-format off
        const Mat4 translation_mat{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            // Invert Y since vulkan renders inverted
            // FIXME: This should be done in shader
            position.x, -position.y, depth, 1
        };
        const Mat4 rotation_mat{
            std::cos(rotation), -std::sin(rotation), 0, 0,
            std::sin(rotation), std::cos(rotation), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        const Mat4 scale_mat{
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        // clang-format on

        return translation_mat * rotation_mat * scale_mat;
    }
};

} // namespace kzn