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


VkCommandBuffer Renderer::begin_frame()
{
    if(_frame_started)
    {
        throw std::runtime_error("Can't call beginFrame while already in progress");
    }

    // Aquire next image for rendering
    auto result = _swap_chain->acquire_next_image(&_current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swap_chain();
        return nullptr;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    _frame_started = true;

    auto command_buffer = _command_buffers[_current_frame_index]; //getCurrentCommandBuffer();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return command_buffer;
}


void Renderer::end_frame()
{
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call endFrame while frame is not in progress");
    }

    auto command_buffer = _command_buffers[_current_frame_index];
    if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = _swap_chain->submit_command_buffers(&command_buffer, &_current_image_index);
    if(result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || _window.was_resized())
    {
        _window.reset_resized_flag();
        recreate_swap_chain();
    }
    else if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _frame_started = false;
    _current_frame_index = (_current_frame_index + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}


void Renderer::begin_render_pass(VkCommandBuffer command_buffer)
{
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call 'begin_render_pass' if frame is not in progress");
    }

    if(command_buffer != _command_buffers[_current_frame_index])
    {
        throw std::runtime_error("Can't begin render pass on command buffer from a different frame");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = _swap_chain->render_pass();
    render_pass_info.framebuffer = _swap_chain->framebuffer(_current_image_index);

    const auto swap_chain_extent = _swap_chain->swap_chain_extent();
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swap_chain_extent;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    
    viewport.width = static_cast<float>(swap_chain_extent.width);
    viewport.height = static_cast<float>(swap_chain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swap_chain_extent };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}


void Renderer::end_render_pass(VkCommandBuffer command_buffer)
{
    if(!_frame_started)
    {
        throw std::runtime_error("Can't call 'end_render_pass' if frame is not in progress");
    }

    if(command_buffer != _command_buffers[_current_frame_index])
    {
        throw std::runtime_error("Can't end render pass on command buffer from a different frame");
    }

    vkCmdEndRenderPass(command_buffer);
}

}
