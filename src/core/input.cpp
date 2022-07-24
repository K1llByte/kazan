#include "core/input.hpp"

namespace kzn
{
    Input::Input(Window& window)
        : glfw_window(window.glfw_ptr()) {}

    Input::~Input() = default;
    
    int Input::get_key(int key_code) const {
        return glfwGetKey(glfw_window, key_code);
    }

    bool Input::is_key_pressed(int key_code) const {
        return this->get_key(key_code) == GLFW_PRESS;
    }

    bool Input::is_key_released(int key_code) const {
        return this->get_key(key_code) == GLFW_RELEASE;
    }

    int Input::get_mouse_button(int button) const {
        return glfwGetMouseButton(glfw_window, button);
    }

    CursorPos Input::get_cursor_pos(int /*key_code*/) const {
        double x;
        double y;
        glfwGetCursorPos(glfw_window, &x, &y);
        return CursorPos {
            .x = static_cast<float>(x),
            .y = static_cast<float>(y)
        };
    }
} // namespace kzn