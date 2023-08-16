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
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   present_modes;

    VkSurfaceFormatKHR select_format() const;
    VkPresentModeKHR select_present_mode() const;
    VkExtent2D select_extent(VkExtent2D extent) const;
};

struct DeviceParams {
    std::vector<char const*> extensions = {};
    VkPhysicalDeviceFeatures features = {};
    VkSurfaceKHR             surface = VK_NULL_HANDLE;
};

class Device {
public:
    // Ctor
    Device(Instance& instance, DeviceParams&& params = {});
    // Copy
    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;
    // Move
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
    // Dtor
    ~Device();

    // Getters
    VkDevice vk_device() { return m_vk_device; }
    constexpr VkQueue graphics_queue() { return m_vk_graphics_queue; }
    constexpr VkQueue present_queue() { return m_vk_present_queue; }

    const SwapchainSupport& swapchain_support() const { return m_swapchain_support; }
    const QueueFamilies& queue_families() const { return m_queue_families; }

private:
    vk::Instance&    m_instance;
    VkDevice         m_vk_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
    VkQueue          m_vk_graphics_queue;
    VkQueue          m_vk_present_queue;
    SwapchainSupport m_swapchain_support;
    QueueFamilies    m_queue_families;
    VmaAllocator     m_vma_allocator;
};

} // namespace kzn::vk