#include "controllers/camera_controller.hpp"

// #include "utils.hpp"
#include <glm/gtc/constants.hpp>

#include <algorithm>

// Debug
#include <iostream>

namespace kzn
{

// TODO: Move to utils
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
    constexpr float move_speed = 1.5f;
    constexpr float look_speed = 1.5f;
    constexpr float sensibility = 0.7f;
    glm::vec3 pos = _camera.position();
    glm::vec3 dir = _camera.direction();
    glm::vec3 up{0.f, -1.f, 0.f}; //= _camera.up();
    glm::vec3 right = glm::normalize(glm::cross(dir, up));

    // Positional movement
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

    // Direction rotation: Keys
    if(_window.get_key(key_maps.look_right) == GLFW_PRESS)
        rotation.y += look_speed * dt;
    if(_window.get_key(key_maps.look_left) == GLFW_PRESS)
        rotation.y -= look_speed * dt;
    if(_window.get_key(key_maps.look_up) == GLFW_PRESS)
        rotation.x += look_speed * dt;
    if(_window.get_key(key_maps.look_down) == GLFW_PRESS)
        rotation.x -= look_speed * dt;

    // Aux data
    const VkExtent2D extent = _window.get_extent();
    const auto [w, h] = extent;
    const VkExtent2D half_extent { w / 2, h / 2};
    const auto [hw, hh] = half_extent;


    // Disable mouse (unique press)
    static bool use_mouse = true;
    static bool disable_mouse_press = false;
    if(_window.get_key(key_maps.disable_mouse) == GLFW_PRESS)
    {
        disable_mouse_press = true;
    }
    if(disable_mouse_press && _window.get_key(key_maps.disable_mouse) == GLFW_RELEASE)
    {
        use_mouse = !use_mouse;
        disable_mouse_press = !disable_mouse_press;
        _window.set_cursor_position(hw, hh);
    }
    
    // Direction rotation: Mouse
    if(use_mouse)
    {
        auto [x, y] = _window.get_cursor_position();
        const float fovy = glm::radians(50.f);
        const float aspect = w / h;
        rotation.y -= ((hw - x) / hw) * fovy * sensibility;
        rotation.x -= ((hh - y) / hh) * fovy * aspect * sensibility;

        _window.set_cursor_position(hw, hh);
    }

    const float clamp_error = 0.000001f;
    dir = to_spherical(dir);
    dir.y = std::clamp<float>(dir.y + rotation.y, -glm::pi<float>(), glm::pi<float>());
    dir.z = std::clamp<float>(dir.z + rotation.x, 0, glm::pi<float>() - clamp_error);
    dir = to_cartesian(dir);

    // right = to_spherical(right);
    // right.y = std::clamp<float>(right.y + rotation.y, -glm::pi<float>(), glm::pi<float>());
    // right.z = std::clamp<float>(right.z + rotation.x, 0, glm::pi<float>() - clamp_error);
    // right = to_cartesian(right);

    _camera.lookat_direction(pos, dir, up);
}

}