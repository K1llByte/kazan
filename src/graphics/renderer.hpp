#pragma once

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "vk/cmd_buffer.hpp"
#include "core/window.hpp"

#include <functional>

namespace kzn {

struct PerFrameData {
public:
    vk::CommandBuffer cmd_buffer;
    VkSemaphore       img_available;
    VkSemaphore       finished_render;
    VkFence           in_flight_fence;

    // Ctor
    PerFrameData(vk::Device& device, vk::CommandPool& cmd_pool);
    // Copy
    PerFrameData(const PerFrameData&) = delete;
    PerFrameData& operator=(const PerFrameData&) = delete;
    // Move
    PerFrameData(PerFrameData&&) = default;
    PerFrameData& operator=(PerFrameData&&) = default;
    // Dtor
    ~PerFrameData();

private:
    vk::Device& m_device;
};

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
    ~Renderer() = default;

    void render_frame(const RenderFrameFunc& render_func);
    
private:
    vk::Device&       m_device;
    vk::Swapchain&    m_swapchain;
    vk::CommandPool   m_cmd_pool;
    Window&           m_window;
    // Sync
    constexpr static size_t   MAX_FRAMES_IN_FLIGHT = 2;
    size_t                    m_frame_idx = 0;
    std::vector<PerFrameData> m_frame_data;

};

} // namespace kzn
