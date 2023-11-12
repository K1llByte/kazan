#pragma once

#include "vk/swapchain.hpp"
#include "vk/dset.hpp"
#include "core/window.hpp"
#include <memory>

namespace kzn {

//! Singleton class for wrapping inner graphics context
class GraphicsContext {
public:
    static void create(Window& window) {
        m_context = std::make_unique<GraphicsContext>(window);
    }

    static void destroy() {
        m_context.reset();
    }

    static vk::Instance& instance() { return m_context->m_instance; }
    static vk::Device& device() { return m_context->m_device; }
    static vk::Swapchain& swapchain() { return m_context->m_swapchain; }
    static vk::DescriptorSetAllocator& dset_allocator() { return m_context->m_dset_allocator; }
    static vk::DescriptorSetLayoutCache& dset_layout_cache() { return m_context->m_dset_layout_cache; }

    GraphicsContext(Window& window)
        : m_instance({
            .extensions = window.required_extensions(),
            .with_validation = true,
        })
        , m_surface(window.create_surface(m_instance))
        , m_device(m_instance, {
            .extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
            .surface = m_surface.vk_surface()
        })
        , m_swapchain(m_device, m_surface, window.extent())
        , m_dset_allocator(m_device)
        , m_dset_layout_cache(m_device)
    {

    }

    ~GraphicsContext() = default;

private:
    static inline std::unique_ptr<GraphicsContext> m_context = nullptr;
    
    vk::Instance                 m_instance;
    vk::Surface                  m_surface;
    vk::Device                   m_device;
    vk::Swapchain                m_swapchain;
    vk::DescriptorSetAllocator   m_dset_allocator;
    vk::DescriptorSetLayoutCache m_dset_layout_cache;
};

}