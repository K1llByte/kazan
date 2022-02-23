#ifndef KZN_VK_DEVICE_HPP
#define KZN_VK_DEVICE_HPP

#include "vk/instance.hpp"

#include <optional>
#include <exception>

namespace kzn::vk
{
    class PhysicalDevice
    {
        friend class PhysicalDeviceSelector;
    public:
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        // std::optional<uint32_t> present_family;

        bool is_complete() const noexcept
        {
            return graphics_family.has_value();
                // && present_family.has_value();
        }
    };

    // struct SwapChainSupportDetails
    // {
    //     VkSurfaceCapabilitiesKHR        capabilities;
    //     std::vector<VkSurfaceFormatKHR> formats;
    //     std::vector<VkPresentModeKHR>   present_modes;
    // };

    class PhysicalDeviceSelector
    {
    public:
        PhysicalDeviceSelector(Instance& instance);
        ~PhysicalDeviceSelector() = default;

        // TODO: Specific selection method
        // like dedicated_gpu() or compute_support()
        PhysicalDevice select();

    private:
        bool is_device_suitable(VkPhysicalDevice physical_device);
        QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);

    private:
        VkInstance                    vkinstance = VK_NULL_HANDLE;
        std::vector<VkPhysicalDevice> available_devices;
        std::vector<const char*>      device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        QueueFamilyIndices            indices;

    };
} // namespace kzn::vk

#endif // KZN_VK_DEVICE_HPP