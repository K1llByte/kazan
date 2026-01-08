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
    Surface(Surface&&);
    Surface& operator=(Surface&&) = default;
    // Dtor
    ~Surface();

public:
    operator VkSurfaceKHR() const { return m_surface; }

private:
    Instance* m_instance_ptr;
    VkSurfaceKHR m_surface;
    // VkSurfaceFormatKHR
    // SurfaceCapabilities
};

} // namespace kzn::vk