#pragma once

#include "GLFW/glfw3.h"
#include "events/event_manager.hpp"
#include "gamepad.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"

#include "core/assert.hpp"
#include "core/window.hpp"
#include "math/types.hpp"

namespace kzn {

enum class InputAction : int {
    Press = GLFW_PRESS,
    Release = GLFW_RELEASE,
};

struct MouseScrollEvent : Event {
    Vec2d scroll;
};

struct KeyboardKeyEvent : Event {
    KeyboardKey key;
    InputAction action;
};

struct CursorPositionEvent : Event {
    Vec2d position;
};

struct MouseButtonEvent : Event {
    MouseButton button;
    InputAction action;
};

struct GamepadConnectedEvent : Event {
    GamepadId gamepad_id;
    bool connected;
};

class Input {
public:
    // Ctor
    Input(Window& window)
        : m_window_ptr{&window}
        , m_keyboard(window)
        , m_mouse(window)
        , m_gamepads{} {
        // Attach a reference of this input instance to the window
        // This is used to allow user data of glfw callbacks to have access to
        // this input data since the window is the main user data.
        m_window_ptr->set_input(this);

        // Set glfw scroll callback
        glfwSetScrollCallback(window.glfw_ptr(), &Input::scroll_callback);
        // Set glfw key callback
        glfwSetKeyCallback(window.glfw_ptr(), &Input::key_callback);
        // Set glfw mouse position change callback
        glfwSetCursorPosCallback(
            window.glfw_ptr(), &Input::cursor_position_callback
        );
        // Set glfw gamepad connected callback
        glfwSetJoystickCallback(&Input::joystick_callback);
    }
    // Copy
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    // Move
    Input(Input&&) = delete;
    Input& operator=(Input&&) = delete;
    // Dtor
    ~Input() {
        // Dettach this instance from window
        m_window_ptr->set_input(nullptr);
    }

    [[nodiscard]]
    const Keyboard& keyboard() const {
        return m_keyboard;
    }

    [[nodiscard]]
    const Mouse& mouse() const {
        return m_mouse;
    }

    [[nodiscard]]
    const Gamepad& gamepad(GamepadId gamepad_id) const {
        KZN_ASSERT_MSG(
            static_cast<int>(gamepad_id) < gamepad_count, "Invalid GamepadId"
        );
        return m_gamepads[(static_cast<int>(gamepad_id))];
    }

    [[nodiscard]]
    bool gamepad_connected(GamepadId gamepad_id) {
        return glfwJoystickPresent(static_cast<int>(gamepad_id));
    }

    //! Updates state data of input devices (delta values and gamepads state)
    void update_state() {
        // Update gamepads state
        for (std::size_t i = 0; i < gamepad_count; ++i) {
            glfwGetGamepadState(i, &(m_gamepads[i].m_state));
        }

        // Update mouse state
        m_mouse.m_prev_position = m_mouse.m_position;
        glfwGetCursorPos(
            m_window_ptr->glfw_ptr(),
            &m_mouse.m_position.x,
            &m_mouse.m_position.y
        );
    }

private:
    friend class Window;

    static void scroll_callback(
        GLFWwindow* glfw_window_ptr,
        double xoffset,
        double yoffset
    ) {
        // Get glfw user data
        auto window_ptr =
            static_cast<Window*>(glfwGetWindowUserPointer(glfw_window_ptr));
        // Update mouse scroll
        window_ptr->input()->m_mouse.set_scroll(Vec2d{xoffset, yoffset});
        EventManager::send(MouseScrollEvent{.scroll = Vec2d{xoffset, yoffset}});
    }

    static void key_callback(
        GLFWwindow* glfw_window_ptr,
        int key,
        int scan_code,
        int action,
        int mods
    ) {
        EventManager::send(KeyboardKeyEvent{
            .key = KeyboardKey{key},
            .action = InputAction{action},
        });
    }

    static void cursor_position_callback(
        GLFWwindow* glfw_window_ptr,
        double xpos,
        double ypos
    ) {
        EventManager::send(CursorPositionEvent{
            .position = Vec2d{xpos, ypos},
        });
    }

    static void mouse_button_callback(
        GLFWwindow* glfw_window_ptr,
        int button,
        int action,
        int mods
    ) {
        EventManager::send(MouseButtonEvent{
            .button = MouseButton{button},
            .action = InputAction{action},
        });
    }

    static void joystick_callback(int jid, int event) {
        EventManager::send(GamepadConnectedEvent{
            .connected = event == GLFW_CONNECTED,
        });
    }

private:
    Window* m_window_ptr;
    Keyboard m_keyboard;
    Mouse m_mouse;
    std::array<Gamepad, gamepad_count> m_gamepads;
};

} // namespace kzn