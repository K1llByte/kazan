#ifndef KZN_INPUT_HPP
#define KZN_INPUT_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace kzn
{
    struct CursorPos
    {
        float x, y;
    };

    class Input
    {
    public:
        Input(GLFWwindow* glfw_window);
        ~Input() = default;

        // Keyboard
        int get_key(int key_code) const;
        bool is_key_pressed(int key_code) const;
        bool is_key_released(int key_code) const;
        // Mouse
        int get_mouse_button(int button) const;
        CursorPos get_cursor_pos(int key_code) const;

    private:
        GLFWwindow* glfw_window;
    };
} // namespace kzn


#endif // KZN_INPUT_HPP