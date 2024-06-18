#pragma once

#include "core/traits.hpp"
#include "vk/dset.hpp"
#include "vk/dset_layout.hpp"
#include <cassert>
#include <core/window.hpp>
#include <events/event_manager.hpp>
#include <optional>
#include <vk/cmd_buffer.hpp>
#include <vk/device.hpp>
#include <vk/swapchain.hpp>

#include <functional>

namespace kzn {

struct PerFrameData {
public:
    vk::CommandBuffer cmd_buffer;
    VkSemaphore img_available;
    VkSemaphore finished_render;
    VkFence in_flight_fence;

    // Ctor
    PerFrameData(vk::Device& device, vk::CommandPool& cmd_pool);
    // Copy
    PerFrameData(const PerFrameData&) = delete;
    PerFrameData& operator=(const PerFrameData&) = delete;
    // Move
    PerFrameData(PerFrameData&&);
    PerFrameData& operator=(PerFrameData&&) = default;
    // Dtor
    ~PerFrameData();

private:
    vk::Device& m_device;
};

//! Renderer events
struct SwapchainResize : Event {};

class Renderer : public Singleton<Renderer> {
public:
    using RenderFrameFunc = std::function<void(vk::CommandBuffer&)>;

    // Ctor
    Renderer(Window& window);
    // Copy
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Move
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    // Dtor
    ~Renderer() = default;

    void render_frame(const RenderFrameFunc& render_func);

    // Singleton aware static functions //
    [[nodiscard]]
    static vk::Instance& instance() {
        assert(exists());
        return singleton().m_instance;
    }
    [[nodiscard]]
    static vk::Device& device() {
        assert(exists());
        return singleton().m_device;
    }
    [[nodiscard]]
    static vk::Swapchain& swapchain() {
        assert(exists());
        return singleton().m_swapchain;
    }
    [[nodiscard]]
    static vk::DescriptorSetAllocator& dset_allocator() {
        assert(exists());
        return singleton().m_dset_allocator;
    }
    [[nodiscard]]
    static vk::DescriptorSetLayoutCache& dset_layout_cache() {
        assert(exists());
        return singleton().m_dset_layout_cache;
    }

private:
    // Members
    Window& m_window;
    vk::Instance m_instance;
    vk::Surface m_surface;
    vk::Device m_device;
    vk::Swapchain m_swapchain;
    vk::DescriptorSetAllocator m_dset_allocator;
    vk::DescriptorSetLayoutCache m_dset_layout_cache;
    vk::CommandPool m_cmd_pool;

    // Synchronization data
    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    size_t m_frame_idx = 0;
    std::vector<PerFrameData> m_frame_data;

private:
    // Auxiliary function to ensure renderer is only initialized once.
    Window& create_context_check(Window& window);
};

} // namespace kzn
