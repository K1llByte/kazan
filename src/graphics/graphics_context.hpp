#pragma once

#include "vk/swapchain.hpp"
#include "core/window.hpp"
#include <memory>

namespace kzn {

//! Singleton class for wrapping inner graphics context
class GraphicsContext {
public:
    static void create(Window& window) {
        m_context = new GraphicsContext(window);
    }

    static void destroy() {
        delete m_context;
    }

    static vk::Instance& instance() { return m_context->m_instance; }
    static vk::Device& device() { return m_context->m_device; }
    static vk::Swapchain& swapchain() { return m_context->m_swapchain; }

    ~GraphicsContext() = default;

private:
    static inline GraphicsContext* m_context = nullptr;
    
    vk::Instance  m_instance;
    vk::Surface   m_surface;
    vk::Device    m_device;
    vk::Swapchain m_swapchain;

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
    {

    }
};

}