#pragma once

#include "vk/instance.hpp"

namespace kzn::vk {

class Surface {        
public:
    // Ctor
    Surface(Instance& instance, VkSurfaceKHR surface);
    // Copy
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    // Move
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;
    // Dtor
    ~Surface();

public:
    VkSurfaceKHR vk_surface() { return m_surface; }

private:
    Instance*    m_instance;
    VkSurfaceKHR m_surface;
    // VkSurfaceFormatKHR
    // SurfaceCapabilities
};

} // namespace kzn::vk