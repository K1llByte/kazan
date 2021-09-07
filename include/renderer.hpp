#ifndef RENDERER_H
#define RENDERER_H

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include <vector>

namespace kzn
{

class Renderer
{

public:

    Window&                      _window;
    Device&                      _device;
    SwapChain*                   _swap_chain = nullptr;
    std::vector<VkCommandBuffer> _command_buffers;

public:

    Renderer(Window& window, Device& device);
    ~Renderer();

    void recreate_swap_chain();

};

}

#endif // RENDERER_H
