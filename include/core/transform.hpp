#ifndef KZN_TRANSFORM_HPP
#define KZN_TRANSFORM_HPP

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

namespace kzn
{
    struct TransformComponent
    {
        glm::vec3 position{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};
    
        glm::mat4 mat4() const
        {
            // auto transform = glm::translate(glm::mat4{1.f}, position);
            // transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
            // transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
            // transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });
            // transform = glm::scale(transform, scale);
            // return transform;

            // Equivalent code, but faster
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            // std::cout << position.x << ", " << position.y << ", " << position.z << '\n';
            return glm::mat4{
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                {position.x, position.y, position.z, 1.0f}
            };
        }
    };
} // namespace kzn

#endif // KZN_TRANSFORM_HPP