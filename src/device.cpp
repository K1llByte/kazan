#include "device.hpp"

#include <stdexcept>
#include <iostream>
#include <set>

namespace kzn
{

PhysicalDevice::PhysicalDevice(
        VkPhysicalDevice physical_device,
        QueueFamilyIndices&& indices,
        std::vector<const char*>&& device_extensions,
        SwapChainSupportDetails swap_chain_support)
    : _physical_device{physical_device},
    _indices{std::move(indices)},
    _device_extensions{std::move(device_extensions)},
    _swap_chain_support{std::move(swap_chain_support)}
{
    
}


PhysicalDevice::~PhysicalDevice()
{

}


PhysicalDeviceSelector::PhysicalDeviceSelector(Instance& instance, VkSurfaceKHR surface)
    : m_instance{instance},
    m_surface{surface}
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance._instance, &device_count, nullptr);

    if (device_count == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    m_available_devices.resize(device_count);
    vkEnumeratePhysicalDevices(m_instance._instance, &device_count, m_available_devices.data());
}


bool PhysicalDeviceSelector::is_device_suitable(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        return false;

    /* QueueFamilyIndices indices = */
    find_queue_families(physical_device);

    bool extensions_supported = check_device_extension_support(physical_device);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        _swap_chain_support = query_swap_chain_support(physical_device);
        swap_chain_adequate = !_swap_chain_support.formats.empty() && !_swap_chain_support.present_modes.empty();
    }

    return _indices.is_complete() && swap_chain_adequate;
}


bool PhysicalDeviceSelector::check_device_extension_support(VkPhysicalDevice physical_device)
{
    // 'vkEnumerateDeviceExtensionProperties' will fetch all supported device extensions
    // and this function iterates over all 'device_extensions' that are wanted to be loaded,
    // and check if they're in the 'available_extensions' vector fetched from the vulkan call
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(_device_extensions.begin(), _device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}


SwapChainSupportDetails PhysicalDeviceSelector::query_swap_chain_support(VkPhysicalDevice physical_device)
{
    SwapChainSupportDetails details;

    // 1. VkSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &details.capabilities);

    // 2. VkSurfaceFormatKHR's
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, details.formats.data());
    }

    // 3. VkPresentModeKHR's
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}


QueueFamilyIndices PhysicalDeviceSelector::find_queue_families(VkPhysicalDevice physical_device)
{
    // QueueFamilyIndices indices;

    // Logic to find queue family indices to populate struct with
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    for(size_t i = 0 ; i < queue_families.size() ; ++i)
    {
        if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_support);
        if(present_support)
            _indices.present_family = i;

        if(_indices.is_complete())
            break;
    }

    return _indices;
}


PhysicalDevice PhysicalDeviceSelector::select()
{
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (const auto& device : m_available_devices)
    {
        if (is_device_suitable(device))
        {
            physical_device = device;
            break;
        }
    }

    if(physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    else
    {
        std::cout << "+ Physical device selected successfully\n";

        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);
        std::cout << "[INFO] > Using " << device_properties.deviceName << "\n";

        return PhysicalDevice(
            physical_device,
            std::move(_indices),
            std::move(_device_extensions),
            std::move(_swap_chain_support)
            );
    }
}


Device::Device(const PhysicalDevice& physical_device)
    : _physical_device{physical_device._physical_device},
    _indices{physical_device._indices},
    _swap_chain_support{physical_device._swap_chain_support},
    _device_extensions{physical_device._device_extensions}
{
    
}


Device::~Device()
{
    if(initialized())
    {
        cleanup();
    }
}


void Device::init()
{
    _validation_layers.push_back("VK_LAYER_KHRONOS_validation");

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        _indices.graphics_family.value(),
        _indices.present_family.value()
    };
    
    float queue_priority = 1.0f;
    for(uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = static_cast<uint32_t>(_device_extensions.size());
    create_info.ppEnabledExtensionNames = _device_extensions.data();

    if(true)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(_validation_layers.size());
        create_info.ppEnabledLayerNames = _validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(_physical_device, &create_info, nullptr, &_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(_device, _indices.graphics_family.value(), 0, &_graphics_queue);
    vkGetDeviceQueue(_device, _indices.present_family.value(), 0, &_present_queue);

    // Create command buffer
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = _indices.graphics_family.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
                    | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if(vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}


void Device::cleanup()
{
    if(initialized())
    {
        // Destroy command pool
        vkDestroyCommandPool(_device, _command_pool, nullptr);
        
        // Destroy logical device instance
        vkDestroyDevice(_device, nullptr);
    }
    else
    {
        throw std::runtime_error("Cannot cleanup unitialized device");
    }
}


bool Device::initialized() const
{
    return _device != VK_NULL_HANDLE;
}


void Device::wait_idle()
{
    vkDeviceWaitIdle(_device);
}


VkQueue Device::graphics_queue() const
{
    return _graphics_queue;
}


VkQueue Device::present_queue() const
{
    return _present_queue;
}


VkDevice Device::device() const
{
    return _device;
}


VkCommandPool Device::command_pool() const
{
    return _command_pool;
}


uint32_t Device::find_memory_type(
    uint32_t type_filter,
    VkMemoryPropertyFlags properties)
{
    // TODO: Understand this later
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
    {
        if ((type_filter & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

  throw std::runtime_error("failed to find suitable memory type!");
}


void Device::create_image_with_info(
    const VkImageCreateInfo &image_info,
    VkMemoryPropertyFlags properties,
    VkImage &image,
    VkDeviceMemory &image_memory)
{
    if(vkCreateImage(_device, &image_info, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(_device, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

    if(vkAllocateMemory(_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    if(vkBindImageMemory(_device, image, image_memory, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to bind image memory!");
    }
}

} // namespace kzn