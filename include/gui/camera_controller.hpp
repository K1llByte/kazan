#ifndef KZN_GUI_CAMERA_CONTROLLER_HPP
#define KZN_GUI_CAMERA_CONTROLLER_HPP

#include "core/camera.hpp"
#include "core/window.hpp"
#include "core/input.hpp"

namespace kzn
{
    struct KeyMapping
    {
        int move_front = GLFW_KEY_W;
        int move_back  = GLFW_KEY_S;
        int move_right = GLFW_KEY_D;
        int move_left  = GLFW_KEY_A;
        int move_up    = GLFW_KEY_E;
        int move_down  = GLFW_KEY_Q;

        int look_up    = GLFW_KEY_UP;
        int look_down  = GLFW_KEY_DOWN;
        int look_right = GLFW_KEY_RIGHT;
        int look_left  = GLFW_KEY_LEFT;

        int disable_mouse = GLFW_KEY_ESCAPE;
    };

    class CameraController
    {
    public:
        CameraController(Window* _window, Camera* _camera)
            : window(_window), camera(_camera), input(*window) {}

        void update(float delta_time);
    
    private:
        Window*    window;
        Camera*    camera = nullptr;
        Input      input;
        KeyMapping key_maps{};
    };
} // namespace kzn

#endif // KZN_GUI_CAMERA_CONTROLLER_HPP