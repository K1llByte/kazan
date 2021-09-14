#include "camera_controller.hpp"

// #include "utils.hpp"
#include <glm/gtc/constants.hpp>

#include <algorithm>

// Debug
#include <iostream>

namespace kzn
{

glm::vec3 to_spherical(glm::vec3 coords)
{
    const float x = coords.x;
    const float y = coords.y;
    const float z = coords.z;
    return glm::vec3{
        sqrt(x*x + y*y + z*z),
        atan2(x,z), // [0, pi]
        acos(y / sqrt(x*x + y*y + z*z)) // [0, 2pi]
    };
}


glm::vec3 to_cartesian(glm::vec3 coords)
{
    const float p = coords.x;
    const float theta = coords.y;
    const float phi = coords.z;
    return glm::vec3{
        sin(theta) * sin(phi) * p,
        cos(phi) * p,
        sin(phi) * p * cos(theta)
    };
}

void CameraController::update(float dt)
{
    constexpr float move_speed = 1.0f;
    constexpr float look_speed = 1.0f;
    glm::vec3 pos = _camera.position();
    glm::vec3 dir = _camera.direction();
    glm::vec3 up{0.f, -1.f, 0.f}; //= _camera.up();
    glm::vec3 right = glm::normalize(glm::cross(dir, up));

    if(_window.get_key(key_maps.move_front) == GLFW_PRESS)
        pos += move_speed * dt * dir;
    if(_window.get_key(key_maps.move_back) == GLFW_PRESS)
        pos -= move_speed * dt * dir;
    if(_window.get_key(key_maps.move_right) == GLFW_PRESS)
        pos += move_speed * dt * right;
    if(_window.get_key(key_maps.move_left) == GLFW_PRESS)
        pos -= move_speed * dt * right;
    if(_window.get_key(key_maps.move_up) == GLFW_PRESS)
        pos += move_speed * dt * up;
    if(_window.get_key(key_maps.move_down) == GLFW_PRESS)
        pos -= move_speed * dt * up;

    glm::vec2 rotation{0.f};

    if(_window.get_key(key_maps.look_right) == GLFW_PRESS)
        rotation.y += look_speed * dt;
    if(_window.get_key(key_maps.look_left) == GLFW_PRESS)
        rotation.y -= look_speed * dt;
    if(_window.get_key(key_maps.look_up) == GLFW_PRESS)
        rotation.x += look_speed * dt;
    if(_window.get_key(key_maps.look_down) == GLFW_PRESS)
        rotation.x -= look_speed * dt;
    

    const float clamp_error = 0.000001f;
    dir = to_spherical(dir);
    dir.y = std::clamp<float>(dir.y + rotation.y, -glm::pi<float>(), glm::pi<float>());
    dir.z = std::clamp<float>(dir.z + rotation.x, 0, glm::pi<float>() - clamp_error);
    // dir.y += rotation.y;
    // dir.z += rotation.x;
    dir = to_cartesian(dir);

    // right = to_spherical(right);
    // right.y = std::clamp<float>(right.y + rotation.y, -glm::pi<float>(), glm::pi<float>());
    // right.z = std::clamp<float>(right.z + rotation.x, 0, glm::pi<float>() - clamp_error);
    // right = to_cartesian(right);

    _camera.lookat_direction(pos, dir, up);
}

}
