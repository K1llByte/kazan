#ifndef DEVICE
#define DEVICE

#include "instance.hpp"

#include <optional>

// FIXME:
// - 'device_builder.m_physical_device.m_physical_device' this
// is happening in the code, meaning that this file isn't pretty 

namespace kzn
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete()
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};


class PhysicalDeviceSelector;


class PhysicalDevice
{
public:

    VkPhysicalDevice m_physical_device;
    QueueFamilyIndices m_indices;
    std::vector<const char*> m_device_extensions;

public:

    PhysicalDevice(
        VkPhysicalDevice           physical_device,
        QueueFamilyIndices&&       indices,
        std::vector<const char*>&& device_extensions);

    ~PhysicalDevice();

    PhysicalDevice(const PhysicalDevice&) = delete;
    void operator=(const PhysicalDevice&) = delete;
};


class PhysicalDeviceSelector
{
public:

    Instance& m_instance;
    std::vector<VkPhysicalDevice> m_available_devices;
    VkSurfaceKHR m_surface;
    std::vector<const char*> m_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    QueueFamilyIndices m_indices;
    SwapChainSupportDetails m_swap_chain_support;
    
public:

    PhysicalDeviceSelector(Instance& instance, VkSurfaceKHR surface = VK_NULL_HANDLE);

    // PhysicalDeviceSelector& set_surface(VkSurfaceKHR surface);

    PhysicalDevice select();

private:

    bool is_device_suitable(VkPhysicalDevice physical_device);

    bool check_device_extension_support(VkPhysicalDevice physical_device);

    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device);

    QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);
};


class DeviceBuilder;


class Device
{
public:

    VkPhysicalDevice m_physical_device;
    QueueFamilyIndices m_indices;
    // std::vector<const char*> m_device_extensions;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
    VkDevice m_device;
    
public:

    // Device();

    Device(DeviceBuilder&& device_builder);

    Device(Device&& device) = default;

    ~Device();

    // Device(const Device&) = delete;
    // void operator=(const Device&) = delete;
};


class DeviceBuilder
{
public:

    PhysicalDevice& m_physical_device;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
    VkDevice m_device;
    std::vector<const char*> m_validation_layers;

public:

    DeviceBuilder(PhysicalDevice& physical_device);

    Device build();
};

} // namespace kzn

#endif // DEVICE