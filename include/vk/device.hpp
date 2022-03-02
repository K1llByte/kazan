#ifndef KZN_VK_DEVICE_HPP
#define KZN_VK_DEVICE_HPP

#include "vk/instance.hpp"

#include <optional>
#include <exception>

namespace kzn::vk
{
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

    // struct SwapChainSupportDetails
    // {
    //     VkSurfaceCapabilitiesKHR        capabilities;
    //     std::vector<VkSurfaceFormatKHR> formats;
    //     std::vector<VkPresentModeKHR>   present_modes;
    // };

    class Device
    {
        friend class DeviceBuilder;
    public:
        ~Device();

        VkDevice vk_device() noexcept { return vkdevice; }

    private:
        Device() = default;

    private:
        VkDevice vkdevice;
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
        VkQueue                       graphics_queue = VK_NULL_HANDLE;
        VkQueue                       present_queue = VK_NULL_HANDLE;
        VkSurfaceKHR                  surface = VK_NULL_HANDLE;
    };
} // namespace kzn::vk

#endif // KZN_VK_DEVICE_HPP