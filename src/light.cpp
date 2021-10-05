#include "light.hpp"

namespace kzn
{

Light DirLight(
    const glm::vec3& direction,
    const glm::vec3& ambient, 
    const glm::vec3& diffuse,
    const glm::vec3& specular)
{
    return Light{
        .type = Light::Type::DIR_LIGHT,

        .ambient = ambient,
        .diffuse = diffuse,
        .specular = specular,

        .direction = direction,
    };
}


Light PointLight(
    const glm::vec3& position,
    const float constant,
    const float linear,
    const float quadratic,
    const glm::vec3& ambient, 
    const glm::vec3& diffuse,
    const glm::vec3& specular)
{
    return std::move(Light{
        .type = Light::Type::POINT_LIGHT,

        .ambient = ambient,
        .diffuse = diffuse,
        .specular = specular,

        .position = position,
        .constant = constant,
        .linear = linear,
        .quadratic = quadratic,
    });
}


Light SpotLight(
    const glm::vec3& position,
    const glm::vec3& direction,
    const glm::vec3& ambient, 
    const glm::vec3& diffuse,
    const glm::vec3& specular)
{
    return Light{
        .type = Light::Type::SPOT_LIGHT,

        .ambient = ambient,
        .diffuse = diffuse,
        .specular = specular,

        .position = position,
        .direction = direction,
    };
}

}
