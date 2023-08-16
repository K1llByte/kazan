#pragma once

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "vk/cmd_buffer.hpp"
#include "core/window.hpp"

#include <functional>

namespace kzn {

class Renderer {
public:
    using RenderFrameFunc = std::function<void(vk::CommandBuffer&)>;

    // Ctor
    Renderer(
        vk::Device& device,
        vk::Swapchain& swapchain,
        Window& window);
    // Copy
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Move
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    // Dtor
    ~Renderer();

    void render_frame(const RenderFrameFunc& render_func);
    
private:
    vk::Device&       m_device;
    vk::Swapchain&    m_swapchain;
    vk::CommandPool   m_cmd_pool;
    vk::CommandBuffer m_cmd_buffer;
    Window&           m_window;
    // Sync
    VkSemaphore m_image_available; 
    VkSemaphore m_render_finished; 
    VkFence     m_in_flight_fence; 
};

} // namespace kzn
