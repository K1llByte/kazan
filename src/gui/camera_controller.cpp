#include "gui/camera_controller.hpp"

#include "math/coordinates.hpp"
#include "math/constants.hpp"

#include <GLFW/glfw3.h>

namespace kzn
{
    void CameraController::update(float dt)
    {
        constexpr float move_speed = 2.5f;
        constexpr float look_speed = 1.5f;
        // constexpr float sensibility = 0.7f;
        glm::vec3 pos = camera->position();
        glm::vec3 dir = camera->direction();
        glm::vec3 up{0.f, -1.f, 0.f}; //= camera->up();
        glm::vec3 right = glm::normalize(glm::cross(dir, up));

        // Positional movement
        if(input.is_key_pressed(key_maps.move_front))
            pos += move_speed * dt * dir;
        if(input.is_key_pressed(key_maps.move_back))
            pos -= move_speed * dt * dir;
        if(input.is_key_pressed(key_maps.move_right))
            pos += move_speed * dt * right;
        if(input.is_key_pressed(key_maps.move_left))
            pos -= move_speed * dt * right;
        if(input.is_key_pressed(key_maps.move_up))
            pos += move_speed * dt * up;
        if(input.is_key_pressed(key_maps.move_down))
            pos -= move_speed * dt * up;

        glm::vec2 rotation{0.f};

        // Direction rotation: Keys
        if(input.is_key_pressed(key_maps.look_right))
            rotation.y += look_speed * dt;
        if(input.is_key_pressed(key_maps.look_left))
            rotation.y -= look_speed * dt;
        if(input.is_key_pressed(key_maps.look_up))
            rotation.x += look_speed * dt;
        if(input.is_key_pressed(key_maps.look_down))
            rotation.x -= look_speed * dt;

        // Aux data
        // const VkExtent2D extent = window->extent();
        // const auto [w, h] = extent;
        // const VkExtent2D half_extent { w / 2, h / 2};
        // const auto [hw, hh] = half_extent;


        // Disable mouse (unique press)
        // static bool use_mouse = false;
        // static bool disable_mouse_press = false;
        // if(input.is_key_pressed(key_maps.disable_mouse))
        // {
        //     disable_mouse_press = true;
        // }
        // if(disable_mouse_press && input.is_key_pressed(key_maps.disable_mouse) == GLFW_RELEASE)
        // {
        //     use_mouse = !use_mouse;
        //     disable_mouse_press = !disable_mouse_press;
        //     window->set_cursor_position(hw, hh);
        // }
        
        // Direction rotation: Mouse
        // if(use_mouse)
        // {
        //     auto [x, y] = window->get_cursor_position();
        //     const float fovy = glm::radians(50.f);
        //     const float aspect = w / h;
        //     rotation.y -= ((hw - x) / hw) * fovy * sensibility;
        //     rotation.x -= ((hh - y) / hh) * fovy * aspect * sensibility;

        //     window->set_cursor_position(hw, hh);
        // }

        const float clamp_error = 0.000001f;
        dir = math::cartesian_to_spherical(dir);
        dir.y = glm::clamp<float>(dir.y + rotation.y, -math::pi, math::pi);
        dir.z = glm::clamp<float>(dir.z + rotation.x, 0, math::pi - clamp_error);
        dir = math::spherical_to_cartesian(dir);

        // right = to_spherical(right);
        // right.y = std::clamp<float>(right.y + rotation.y, -math::pi, math::pi);
        // right.z = std::clamp<float>(right.z + rotation.x, 0, math::pi - clamp_error);
        // right = to_cartesian(right);

        camera->lookat_direction(pos, dir, up);
    }
} // namespace kzn