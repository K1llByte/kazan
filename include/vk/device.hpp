#ifndef KZN_VK_DEVICE_HPP
#define KZN_VK_DEVICE_HPP

#include "vk/instance.hpp"

#include "vk_mem_alloc.h"

#include <optional>
#include <exception>

namespace kzn::vk
{
    // Forward declarations
    class CommandBuffer;
    class Swapchain;
    
    struct QueueFamilies
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete() const noexcept
        {
            return graphics_family.has_value()
                && present_family.has_value();
        }
    };

    struct SwapChainSupport
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   present_modes;

        VkSurfaceFormatKHR select_format() const noexcept;
        VkPresentModeKHR select_present_mode() const noexcept;
        VkExtent2D select_extent(VkExtent2D extent) const noexcept;
    };

    class Device
    {
        friend class DeviceBuilder;
    public:
        ~Device();

        VkDevice vk_device() noexcept { return vkdevice; }
        VmaAllocator allocator() noexcept { return vma_allocator; }
        // Get cached SwapChainSupport
        const SwapChainSupport& swapchain_support() const noexcept { return swapchain_support_details; }
        const QueueFamilies& queue_families() const noexcept { return queue_family_indices; }

        // Fetch SwapChainSupport for specific surface (updates cached one)
        const SwapChainSupport& query_swapchain_support(VkSurfaceKHR surface) noexcept;
        // TODO: Consider making individual queue handles
        void graphics_queue_submit(
            CommandBuffer& cmd_buffer,
            VkSemaphore wait_semaphore,
            VkSemaphore signal_semaphore,
            VkFence fence);
        void present_queue_present(Swapchain& swapchain, VkSemaphore wait_semaphore);
        void wait_idle() noexcept;

    private:
        Device() = default;

    private:
        VkPhysicalDevice   physical_device;
        VkDevice           vkdevice;
        SwapChainSupport   swapchain_support_details;
        QueueFamilies      queue_family_indices;
        VkQueue            graphics_queue = VK_NULL_HANDLE;
        VkQueue            present_queue = VK_NULL_HANDLE;
        VmaAllocator       vma_allocator;
    };

    class DeviceBuilder
    {
    public:
        DeviceBuilder(Instance& instance);
        ~DeviceBuilder() = default;

        DeviceBuilder& set_surface(VkSurfaceKHR) noexcept;
        DeviceBuilder& set_extensions(const std::vector<const char*>& extensions);
        DeviceBuilder& set_features(const VkPhysicalDeviceFeatures& features) noexcept;

        Device build();

    private:
        std::vector<VkPhysicalDevice> available_devices;
        std::vector<const char*>      validation_layers;
        std::vector<const char*>      device_extensions;
        VkPhysicalDeviceFeatures      device_features{};
        VkPhysicalDevice              vkphysical_device = VK_NULL_HANDLE;
        VkSurfaceKHR                  surface = VK_NULL_HANDLE;
        // Only used for VmaAllocator creation
        VkInstance                    vkinstance;
    };
} // namespace kzn::vk

#endif // KZN_VK_DEVICE_HPP