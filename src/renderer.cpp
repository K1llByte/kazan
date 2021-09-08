#include "renderer.hpp"

#include <stdexcept>

namespace kzn
{

Renderer::Renderer(Window& window, Device& device)
    : _window{window}, _device{device}
{
    recreate_swap_chain();
    create_command_buffers();
}


Renderer::~Renderer()
{
    // Destroy Command Buffers
    vkFreeCommandBuffers(
        _device.device(),
        _device.command_pool(),
        static_cast<uint32_t>(_command_buffers.size()),
        _command_buffers.data());

    // Destroy swapchain
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


void Renderer::create_command_buffers()
{
    _command_buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = _device.command_pool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(_command_buffers.size());

    if(vkAllocateCommandBuffers(_device.device(), &alloc_info, _command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

}
