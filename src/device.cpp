#include "device.hpp"

#include <stdexcept>
#include <iostream>
#include <set>

namespace kzn
{

PhysicalDevice::PhysicalDevice(
        VkPhysicalDevice physical_device,
        QueueFamilyIndices&& indices,
        std::vector<const char*>&& device_extensions)
    : m_physical_device{physical_device},
    m_indices{std::move(indices)},
    m_device_extensions{std::move(device_extensions)} {}


PhysicalDevice::~PhysicalDevice()
{

}


PhysicalDeviceSelector::PhysicalDeviceSelector(Instance& instance, VkSurfaceKHR surface)
    : m_instance{instance},
    m_surface{surface}
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance.m_instance, &device_count, nullptr);

    if (device_count == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    m_available_devices.resize(device_count);
    vkEnumeratePhysicalDevices(m_instance.m_instance, &device_count, m_available_devices.data());
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
        SwapChainSupportDetails swap_chain_support = query_swap_chain_support(physical_device);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }

    return m_indices.is_complete() && swap_chain_adequate;
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

    std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

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
            m_indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_support);
        if(present_support)
            m_indices.present_family = i;

        if(m_indices.is_complete())
            break;
    }

    return m_indices;
}


PhysicalDevice PhysicalDeviceSelector::select()
{
    VkPhysicalDevice physical_device;
    for (const auto& device : m_available_devices)
    {
        if (is_device_suitable(device))
        {
            physical_device = device;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    std::cout << "+ Physical device selected successfully\n";

    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    std::cout << "[INFO] > Using " << device_properties.deviceName << "\n";

    // return PhysicalDevice(std::move(*this), physical_device); // , m_indices
    return PhysicalDevice(physical_device, std::move(m_indices), std::move(m_device_extensions));
}


DeviceBuilder::DeviceBuilder(PhysicalDevice& physical_device)
    : m_physical_device{physical_device} {}

Device DeviceBuilder::build()
{
    // QueueFamilyIndices indices = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        m_physical_device.m_indices.graphics_family.value(),
        m_physical_device.m_indices.present_family.value()
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

    create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
    create_info.ppEnabledExtensionNames = m_device_extensions.data();

    if(true)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(m_device, m_physical_device.m_indices.graphics_family.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_physical_device.m_indices.present_family.value(), 0, &m_present_queue);
}

} // namespace kzn