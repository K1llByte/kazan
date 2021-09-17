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
    SwapChainSupportDetails _swap_chain_support;

public:

    PhysicalDevice(
        VkPhysicalDevice           physical_device,
        QueueFamilyIndices&&       indices,
        std::vector<const char*>&& device_extensions,
        SwapChainSupportDetails swap_chain_support);

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
    SwapChainSupportDetails _swap_chain_support;
    
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


class Device
{
public:

    VkPhysicalDevice         _physical_device;
    QueueFamilyIndices       _indices;
    SwapChainSupportDetails  _swap_chain_support;
    std::vector<const char*> _device_extensions;
    VkQueue                  _graphics_queue;
    VkQueue                  _present_queue;
    VkDevice                 _device = VK_NULL_HANDLE;
    std::vector<const char*> _validation_layers;
    VkCommandPool            _command_pool;
    
public:

    Device() = default;
    Device(const PhysicalDevice& physical_device);
    ~Device();

    void init();
    void cleanup();
    bool initialized() const;

    void wait_idle();

    VkQueue graphics_queue() const;
    VkQueue present_queue() const;
    VkDevice device() const;
    VkCommandPool command_pool() const;

    // Functionality methods

    SwapChainSupportDetails query_swap_chain_support(VkSurfaceKHR surface);

    uint32_t find_memory_type(
        uint32_t type_filter,
        VkMemoryPropertyFlags properties);

    void create_buffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& buffer_memory);

    void copy_buffer(
        VkBuffer src_buffer,
        VkBuffer dst_buffer,
        VkDeviceSize size);

    VkCommandBuffer begin_single_time_commands();
    void end_single_time_commands(VkCommandBuffer command_buffer);


    void create_image_with_info(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);

};

} // namespace kzn

#endif // DEVICE