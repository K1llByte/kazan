#pragma once

#include "vk/instance.hpp"

#include "vk_mem_alloc.h"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace kzn::vk {

struct QueueFamilies {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const {
        return graphics_family.has_value(); //&& present_family.has_value();
    }
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    VkSurfaceFormatKHR select_format() const;
    VkPresentModeKHR select_present_mode() const;
    VkExtent2D select_extent(VkExtent2D extent) const;
};

struct DeviceParams {
    Instance& instance;
    std::vector<char const*> device_extensions = {};
    VkPhysicalDeviceFeatures device_features = {};
};

class Device {
  public:
    // Ctor
    Device(DeviceParams&& params);
    // Copy
    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;
    // Move
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
    // Dtor
    ~Device();

  private:
    vk::Instance* m_instance;
    VkDevice m_vk_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
    VkQueue m_vk_graphics_queue;
    VkQueue m_vk_present_queue;
    VmaAllocator m_vma_allocator;
};

} // namespace kzn::vk