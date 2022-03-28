#include "core/input.hpp"

namespace kzn
{
    Input::Input(Window* _window)
        : window(_window) {}

    int Input::get_key(int key_code) const
    {
        return glfwGetKey(window->glfw_ptr(), key_code);
    }

    bool Input::is_key_pressed(int key_code) const
    {
        return this->get_key(key_code) == GLFW_PRESS;
    }

    int Input::get_mouse_button(int button) const
    {
        return glfwGetMouseButton(window->glfw_ptr(), GLFW_MOUSE_BUTTON_LEFT);
    }

    CursorPos Input::get_cursor_pos(int key_code) const
    {
        double x;
        double y;
        glfwGetCursorPos(window->glfw_ptr(), &x, &y);
        return CursorPos {
            .x = static_cast<float>(x),
            .y = static_cast<float>(y)
        };
    }
} // namespace kzn