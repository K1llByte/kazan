#include "renderer.hpp"

namespace kzn
{

Renderer::Renderer(Window& window, Device& device)
    : _window{window}, _device{device}
{
    recreate_swap_chain();
}


Renderer::~Renderer()
{
    delete _swap_chain;
}


void Renderer::recreate_swap_chain()
{
    auto extent = _window.get_extent();
    while(extent.width == 0 || extent.height == 0)
    {
        extent = _window.get_extent();
        glfwWaitEvents();
    }

    _device.wait_idle();

    if(_swap_chain == nullptr)
    {
        _swap_chain = new SwapChain(_device, extent, _window.surface());
        _swap_chain->init();
    }
    else
    {
        SwapChain* old_swap_chain = _swap_chain;
        _swap_chain = new SwapChain(_device, extent, _window.surface(), old_swap_chain->swap_chain());

        delete old_swap_chain;

        // if (!old_swap_chain->compare_swap_formats(*_swap_chain))
        // {
        //     throw std::runtime_error("Swap chain image(or depth) format has changed!");
        // }
    }
}

}
