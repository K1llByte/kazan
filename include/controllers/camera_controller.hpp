#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "camera.hpp"
#include "window.hpp"

namespace kzn
{

class CameraController
{
public:

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

public:

    KeyMapping key_maps{};

private:

    Camera&    _camera;
    Window&    _window;

public:

    CameraController(Window& win, Camera& cam)
        : _camera{cam}, _window{win} {}

    void update(float dt);
};

}

#endif // CAMERA_CONTROLLER_H
