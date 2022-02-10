#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

namespace kzn
{

struct Light
{
    enum Type: uint
    {
        DIR_LIGHT   = 0,
        POINT_LIGHT = 1,
        SPOT_LIGHT  = 2
    };

    alignas(4) Type type;

    // Common
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
  
    // Spotlight & PointLight
    alignas(16) glm::vec3 position;
    // Spotlight & DirLight
    alignas(16) glm::vec3 direction;

    // PointLight
    alignas(4) float constant;
    alignas(4) float linear;
    alignas(4) float quadratic;
};


Light DirLight(
    const glm::vec3& direction,
    const glm::vec3& ambient = glm::vec3{0.2f}, 
    const glm::vec3& diffuse = glm::vec3{0.8f},
    const glm::vec3& specular = glm::vec3{1.0f});


Light PointLight(
    const glm::vec3& position,
    const float constant,
    const float linear,
    const float quadratic,
    const glm::vec3& ambient = glm::vec3{0.2f}, 
    const glm::vec3& diffuse = glm::vec3{0.8f},
    const glm::vec3& specular = glm::vec3{1.0f});


Light SpotLight(
    const glm::vec3& position,
    const glm::vec3& direction,
    const glm::vec3& ambient = glm::vec3{0.2f}, 
    const glm::vec3& diffuse = glm::vec3{0.8f},
    const glm::vec3& specular = glm::vec3{1.0f});

}

#endif // LIGHT_H
