#include "core/log.hpp"
#include "vk/surface.hpp"

#include <cassert>

namespace kzn::vk {

Surface::Surface(Instance& instance, VkSurfaceKHR surface)
    : m_instance(&instance)
    , m_surface(surface) {
    assert(m_surface != VK_NULL_HANDLE && "Surface cannot be created with null!");
    // TODO: Get format and capabilities
    Log::trace("Surface created");
}

Surface::~Surface() {
    vkDestroySurfaceKHR(m_instance->vk_instance(), m_surface, nullptr);
    Log::trace("Surface destroyed");
}

}