#ifndef DEVICE
#define DEVICE

#include "instance.hpp"

#include <optional>

// FIXME:
// - 'device_builder._physical_device._physical_device' this
// is happening in the code, meaning that this file isn't pretty 

namespace kzn
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() const
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

    VkPhysicalDevice _physical_device;
    QueueFamilyIndices _indices;
    std::vector<const char*> _device_extensions;

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
    std::vector<const char*> _device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    QueueFamilyIndices _indices;
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

    VkPhysicalDevice         _physical_device;
    QueueFamilyIndices       _indices;
    std::vector<const char*> _device_extensions;
    VkQueue                  _graphics_queue;
    VkQueue                  _present_queue;
    VkDevice                 _device;
    std::vector<const char*> _validation_layers;
    
public:

    Device(const PhysicalDevice& physical_device);

    ~Device();

    void init();
    void cleanup();

    bool initialized() const;

    // Device(const Device&) = delete;
    // void operator=(const Device&) = delete;
};

} // namespace kzn

#endif // DEVICE