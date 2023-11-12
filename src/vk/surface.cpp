#include "core/log.hpp"
#include "vk/surface.hpp"

#include <cassert>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

Surface::Surface(Instance& instance, VkSurfaceKHR surface)
    : m_instance(instance)
    , m_surface(surface)
{
    assert(m_surface != VK_NULL_HANDLE && "Surface cannot be created with null!");
    // TODO: Get format and capabilities
    Log::trace("Surface created");
}

Surface::Surface(Surface&& other)
    : m_instance(other.m_instance)
    , m_surface(other.m_surface)
{
    other.m_surface = VK_NULL_HANDLE;
}

Surface::~Surface() {
    if(m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance.vk_instance(), m_surface, nullptr);
        Log::trace("Surface destroyed");
    }
}

}