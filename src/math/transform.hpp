#pragma once

#include "glm/fwd.hpp"
#include "math/types.hpp"

namespace kzn {

struct Transform2DComponent {
    Vec2 position = {0.f, 0.f};
    float rotation = 0.f;
    Vec2 scale = {1.f, 1.f};

    [[nodiscard]]
    constexpr Mat3 matrix() const {
        // NOTE: Initialization of glm matrices is done in column-major.
        // clang-format off
        const Mat3 translation_mat{
            1,0,0,
            0,1,0,
            position.x,position.y,1
        };
        const Mat3 rotation_mat{
            std::cos(rotation), -std::sin(rotation),0,
            std::sin(rotation), std::cos(rotation),0,
            0,0,1
        };
        const Mat3 scale_mat{
            scale.x, 0, 0,
            0, scale.y, 0,
            0, 0, 1
        };
        // clang-format on

        return translation_mat * rotation_mat * scale_mat;
    }
};

} // namespace kzn