#pragma once

#include "core/window.hpp"
#include "math/types.hpp"

namespace kzn {

enum MouseButton : int {
    Button1 = GLFW_MOUSE_BUTTON_1,
    Button2 = GLFW_MOUSE_BUTTON_2,
    Button3 = GLFW_MOUSE_BUTTON_3,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8,
    Left = Button1,
    Right = Button2,
    Middle = Button3
};

class Mouse {
public:
    [[nodiscard]]
    bool is_released(MouseButton button) const {
        return glfwGetMouseButton(m_window_ptr->glfw_ptr(), button) ==
               GLFW_RELEASE;
    }

    [[nodiscard]]
    bool is_pressed(MouseButton button) const {
        return glfwGetMouseButton(m_window_ptr->glfw_ptr(), button) ==
               GLFW_PRESS;
    }

    //! Position in screen coordinates [(0 0),(width, height)]
    [[nodiscard]]
    Vec2d position() const {
        return m_position;
    }

    //! Delta scroll state since last window event poll
    //! x component is the horizontal scroll, mainly affected by touchpad
    //! y component is the vertical scroll
    [[nodiscard]]
    Vec2d scroll() const {
        return m_scroll;
    }

    //! Delta position in screen coordinates
    [[nodiscard]]
    Vec2d delta_position() const {
        return m_position - m_prev_position;
    }

protected:
    friend class Input;

    Mouse(Window& window)
        : m_window_ptr(&window) {}

    void set_scroll(Vec2d scroll) { m_scroll = scroll; }

private:
    Window* m_window_ptr;
    Vec2d m_position;
    Vec2d m_prev_position;
    Vec2d m_scroll;
};

} // namespace kzn