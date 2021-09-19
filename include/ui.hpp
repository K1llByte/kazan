#ifndef UI_H
#define UI_H

#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "renderer.hpp"

namespace kzn
{

class Interface
{
private:

    bool initialized = false;

public:

    Interface() = default;
    Interface(
        Window& window,
        Instance& instance,
        Device& device,
        Renderer& renderer);
    ~Interface();

    void render();
};

}

#endif // UI_H