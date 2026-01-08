#pragma once

#include <GLFW/glfw3.h>
#include <cstddef>

namespace kzn {

enum class GamepadId {
    Gamepad1 = GLFW_JOYSTICK_1,
    Gamepad2 = GLFW_JOYSTICK_2,
    Gamepad3 = GLFW_JOYSTICK_3,
    Gamepad4 = GLFW_JOYSTICK_4,
    Gamepad5 = GLFW_JOYSTICK_5,
    Gamepad6 = GLFW_JOYSTICK_6,
    Gamepad7 = GLFW_JOYSTICK_7,
    Gamepad8 = GLFW_JOYSTICK_8,
    Gamepad9 = GLFW_JOYSTICK_9,
    Gamepad10 = GLFW_JOYSTICK_10,
    Gamepad11 = GLFW_JOYSTICK_11,
    Gamepad12 = GLFW_JOYSTICK_12,
    Gamepad13 = GLFW_JOYSTICK_13,
    Gamepad14 = GLFW_JOYSTICK_14,
    Gamepad15 = GLFW_JOYSTICK_15,
    Gamepad16 = GLFW_JOYSTICK_16,
};

constexpr std::size_t gamepad_count =
    static_cast<std::size_t>(GamepadId::Gamepad16) + 1;

enum class GamepadButton : int {
    ButtonA = GLFW_GAMEPAD_BUTTON_A,
    ButtonB = GLFW_GAMEPAD_BUTTON_B,
    ButtonX = GLFW_GAMEPAD_BUTTON_X,
    ButtonY = GLFW_GAMEPAD_BUTTON_Y,
    LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
    RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    Back = GLFW_GAMEPAD_BUTTON_BACK,
    Start = GLFW_GAMEPAD_BUTTON_START,
    Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
    LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
    RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
    DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
    DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
    DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
    DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
    Last = GLFW_GAMEPAD_BUTTON_LAST,
    Cross = GLFW_GAMEPAD_BUTTON_CROSS,
    Circle = GLFW_GAMEPAD_BUTTON_CIRCLE,
    Square = GLFW_GAMEPAD_BUTTON_SQUARE,
    Triangle = GLFW_GAMEPAD_BUTTON_TRIANGLE,
};

enum class GamepadAxis : int {
    LeftAxisX = GLFW_GAMEPAD_AXIS_LEFT_X,
    LeftAxisY = GLFW_GAMEPAD_AXIS_LEFT_Y,
    RightAxisX = GLFW_GAMEPAD_AXIS_RIGHT_X,
    RightAxisY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
    LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
    RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
};

class Gamepad {
public:
    [[nodiscard]]
    bool is_pressed(GamepadButton button) const {
        return m_state.buttons[static_cast<int>(button)] == GLFW_PRESS;
    }

    [[nodiscard]]
    bool is_released(GamepadButton button) const {
        return m_state.buttons[static_cast<int>(button)] == GLFW_RELEASE;
    }

    [[nodiscard]]
    float axis(GamepadAxis axis) const {
        return m_state.axes[static_cast<int>(axis)];
    }

private:
    friend class Input;

    Gamepad() = default;

private:
    GLFWgamepadstate m_state;
};

} // namespace kzn