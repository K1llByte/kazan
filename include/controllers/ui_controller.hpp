#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "window.hpp"
#include "ui.hpp"

namespace kzn
{

class UIController
{
public:

    struct KeyMapping
    {
        int toggle_ui = GLFW_KEY_ESCAPE;
    };

public:

    KeyMapping key_maps{};

private:

    Interface& _ui;
    Window&    _window;

public:

    UIController(Window& win, Interface& ui)
        : _ui{ui}, _window{win} {}

    void update(float dt);
};

}

#endif // UI_CONTROLLER_H
