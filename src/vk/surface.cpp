#include "vk/surface.hpp"
#include "core/assert.hpp"
#include "core/log.hpp"

#include <vulkan/vulkan_core.h>

namespace kzn::vk {

Surface::Surface(Instance& instance, VkSurfaceKHR surface)
    : m_instance_ptr{&instance}
    , m_surface(surface) {
    KZN_ASSERT_MSG(
        m_surface != VK_NULL_HANDLE, "Surface cannot be created with null"
    );
    // TODO: Get format and capabilities
    Log::trace("Surface created");
}

Surface::Surface(Surface&& other)
    : m_instance_ptr(other.m_instance_ptr)
    , m_surface(other.m_surface) {
    other.m_surface = VK_NULL_HANDLE;
}

Surface::~Surface() {
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(*m_instance_ptr, m_surface, nullptr);
        Log::trace("Surface destroyed");
    }
}

} // namespace kzn::vk