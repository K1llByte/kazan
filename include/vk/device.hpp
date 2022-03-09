#ifndef KZN_VK_DEVICE_HPP
#define KZN_VK_DEVICE_HPP

#include "vk/instance.hpp"
#include "core/log.hpp" // TODO: Delete after debug

#include <optional>
#include <exception>

namespace kzn::vk
{
    // Forward declarations
    class CommandBuffer;
    class Swapchain;
    
    // TODO: Make this struct private within Device
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool is_complete() const noexcept
        {
            return graphics_family.has_value()
                && present_family.has_value();
        }
    };

    // TODO: Move this to swapchain file
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
        const SwapChainSupport& swapchain_support_details() const noexcept { return swapchain_support; }
        const QueueFamilyIndices& queue_families() const noexcept { return queue_family_indices; }

        // TODO: Consider making individual queue handles
        void graphics_queue_submit(
            CommandBuffer& cmd_buffer,
            VkSemaphore wait_semaphore,
            VkSemaphore signal_semaphore,
            VkFence fence);
        void present_queue_present(Swapchain& swapchain, VkSemaphore wait_semaphore) noexcept;
        void wait_idle() noexcept;

    private:
        Device() = default;

    private:
        VkDevice           vkdevice;
        SwapChainSupport   swapchain_support;
        QueueFamilyIndices queue_family_indices;
        // FIXME: If there's support for multiple cmd pools then the 
        // create_command_pool method returns VkCommandPool and theres no
        // member with the cmd pool variable (must be given on cmd buffer creation)
        VkQueue            graphics_queue = VK_NULL_HANDLE;
        VkQueue            present_queue = VK_NULL_HANDLE;
    };

    class DeviceBuilder
    {
    public:
        DeviceBuilder(Instance& instance);
        ~DeviceBuilder() = default;

        DeviceBuilder& set_surface(VkSurfaceKHR);
        DeviceBuilder& set_extensions(const std::vector<const char*>& extensions);

        Device build();

    private:
        std::vector<VkPhysicalDevice> available_devices;
        std::vector<const char*>      validation_layers;
        std::vector<const char*>      device_extensions;
        VkPhysicalDevice              vkphysical_device = VK_NULL_HANDLE;
        VkSurfaceKHR                  surface = VK_NULL_HANDLE;
    };
} // namespace kzn::vk

#endif // KZN_VK_DEVICE_HPP