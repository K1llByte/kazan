#include "controllers/ui_controller.hpp"

namespace kzn
{

void UIController::update(float dt)
{
    static bool toggle_ui_press = false;
    if(_window.get_key(key_maps.toggle_ui) == GLFW_PRESS)
    {
        toggle_ui_press = true;
    }

    if(toggle_ui_press && _window.get_key(key_maps.toggle_ui) == GLFW_RELEASE)
    {
        _ui.toggle();
        toggle_ui_press = !toggle_ui_press;
    }
}

}
