#pragma once

#include "glm/fwd.hpp"
#include "math/types.hpp"
#include <glm/glm.hpp>

namespace kzn {

struct Transform2DComponent {
    Vec2 position = {0.f, 0.f};
    float rotation = 0.f;
    Vec2 scale = {1.f, 1.f};
};

} // namespace kzn