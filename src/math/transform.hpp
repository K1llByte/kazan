#pragma once

#include <glm/glm.hpp>

namespace kzn {

struct Transform2DComponent {
    glm::vec2 position = { 0.f, 0.f };
    float rotation = 0.f;
    glm::vec2 scale = { 1.f, 1.f };
};

}