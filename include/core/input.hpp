#ifndef KZN_INPUT_HPP
#define KZN_INPUT_HPP

#include "core/window.hpp"

#ifndef KZN_WINDOW_GLFW_IMPL
#   error "No Window implementation defined"
#endif

namespace kzn
{
    struct CursorPos {
        float x, y;
    };

    class Input {
        public:
        Input(Window& window);
        ~Input();

        // Keyboard
        int get_key(int key_code) const;
        bool is_key_pressed(int key_code) const;
        bool is_key_released(int key_code) const;
        // Mouse
        int get_mouse_button(int button) const;
        CursorPos get_cursor_pos(int key_code) const;

        private:
#ifdef KZN_WINDOW_GLFW_IMPL
        GLFWwindow* glfw_window;
#else
#   error "No Window implementation defined"
#endif
    };
} // namespace kzn


#endif // KZN_INPUT_HPP