#include "vk/device.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <tuple>
#include <unordered_set>


namespace kzn::vk {

QueueFamilies get_queue_families(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface
) {
    // Logic to find queue family indices to populate struct with
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_count,
        nullptr
    );
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_count,
        queue_families.data()
    );

    QueueFamilies indices;
    for (size_t i = 0; i < queue_families.size(); ++i) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        // The Present Queue support check isn't in an else statement
        // because there could be a queue_family with support for both
        // Graphics and Present
        // VkBool32 present_support = VK_FALSE;
        // vkGetPhysicalDeviceSurfaceSupportKHR(
        //     physical_device,
        //     static_cast<uint32_t>(i),
        //     surface,
        //     &present_support
        // );

        // if (present_support)
        //     indices.present_family = static_cast<uint32_t>(i);

        if (indices.is_complete())
            break;
    }

    return indices;
}

bool supports_extensions(
    VkPhysicalDevice physical_device,
    std::vector<char const*> const& extensions
) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(
        physical_device,
        nullptr,
        &extension_count,
        nullptr
    );
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(
        physical_device,
        nullptr,
        &extension_count,
        available_extensions.data()
    );

    // TODO: Change set creation for a faster check
    std::unordered_set<std::string> required_extensions(
        extensions.begin(),
        extensions.end()
    );
    for (auto const& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }
    return required_extensions.empty();
}

SwapchainSupport get_swapchain_support(
    VkPhysicalDevice vk_physical_device,
    VkSurfaceKHR surface
) {
    SwapchainSupport support;

    // 1. Get Surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        vk_physical_device,
        surface,
        &support.capabilities
    );

    // 2. Get Surface possible formats
    uint32_t formats_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        vk_physical_device,
        surface,
        &formats_count,
        nullptr
    );
    if (formats_count != 0) {
        support.formats.resize(formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            vk_physical_device,
            surface,
            &formats_count,
            support.formats.data()
        );
    }

    // 3. Get Surface possible present modes
    uint32_t present_modes_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        vk_physical_device,
        surface,
        &present_modes_count,
        nullptr
    );
    if (present_modes_count != 0) {
        support.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            vk_physical_device,
            surface,
            &present_modes_count,
            support.present_modes.data()
        );
    }

    return support;
}

using SelectedDevice
    = std::tuple<VkPhysicalDevice, QueueFamilies, SwapchainSupport>;

std::vector<VkPhysicalDevice> get_available_devices(
    vk::Instance& instance
) {
    // List available devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(
        instance.vk_instance(),
        &device_count,
        nullptr
    );
    if (device_count == 0) {
        throw 1; // NoGPUSupport();
    }

    std::vector<VkPhysicalDevice> available_devices(device_count);
    vkEnumeratePhysicalDevices(
        instance.vk_instance(),
        &device_count,
        available_devices.data()
    );
    return available_devices;
}

SelectedDevice select_device(
    vk::Instance& instance,
    std::vector<VkPhysicalDevice> const& available_devices,
    const std::vector<const char*>& required_extensions
) {
    VkPhysicalDevice vk_physical_device;
    QueueFamilies queue_families;
    SwapchainSupport swapchain_support;

    // Find suitable device
    for (auto const& device : available_devices) {
        // Loop body to check if its a suitable device
        // UNUSED:
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        // 1. Require to be a dedicated GPU
        // if (device_properties.deviceType
        //     != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        //     continue;

        // 2. Require to have all needed queue families
        queue_families = get_queue_families(device, VK_NULL_HANDLE); // TODO: later pass the surface handle instead 'surface'
        if (!queue_families.is_complete()) {
            continue;
        }

        // 3. Require to support required device extensions
        if (!supports_extensions(device, required_extensions)) {
            continue;
        }

        // 4. Require SwapChain support
        // TODO:
        // swapchain_support = get_swapchain_support(device, surface);
        // if (swapchain_support.formats.empty()
        //     || swapchain_support.present_modes.empty()) {
        //     continue;
        // }

        // This device is suitable, end loop!
        vk_physical_device = device;
        break;
    }

    if (vk_physical_device == VK_NULL_HANDLE) {
        throw 1; // NoGPUSuitable();
    }
    else {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(vk_physical_device, &device_properties);
        Log::info("Selected GPU: {}", device_properties.deviceName);
    }

    return std::make_tuple(
        vk_physical_device,
        std::move(queue_families),
        std::move(swapchain_support)
    );
}

VkDevice create_device(
    VkPhysicalDevice vk_physical_device,
    QueueFamilies const& queue_families,
    VkPhysicalDeviceFeatures const& features,
    std::vector<char const*> const& extensions
) {
    // TODO: Refactor this, there's no need to create
    // an auxiliar structure, elaborate the
    // QueueFamilies class to provide an iterator
    std::unordered_set<uint32_t> unique_queue_families {
        queue_families.graphics_family.value(),
        //queue_families.present_family.value()
    };

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(
        unique_queue_families.size()
    );

    float queue_priority = 1.0f;
    // for (size_t i = 0; i < unique_queue_families.size(); ++i) {
    size_t i = 0;
    for(const auto family_idx : unique_queue_families) {
        queue_create_infos[i] = VkDeviceQueueCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family_idx,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
    }

    VkDeviceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount
        = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pEnabledFeatures = &features;
    create_info.enabledExtensionCount
        = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDevice vk_device;
    auto result
        = vkCreateDevice(vk_physical_device, &create_info, nullptr, &vk_device);
    VK_CHECK_MSG(result, "Failed to create logical device!");
    Log::trace("Device created");

    return vk_device;
}

Device::Device(
    Instance& instance,
    DeviceParams&& params
) : m_instance(&instance) {
    
    // 1. Select physical device //
    auto available_devices = get_available_devices(*m_instance);
    auto [vk_physical_device, indices, swapchain_support]
        = select_device(*m_instance, available_devices, params.extensions);

    // 2. Create device //
    m_vk_physical_device = vk_physical_device;
    m_vk_device = create_device(
        vk_physical_device,
        indices,
        params.features,
        params.extensions
    );

    // 2.1. Get device queues //
    vkGetDeviceQueue(
        m_vk_device,
        indices.graphics_family.value(),
        0,
        &m_vk_graphics_queue
    );
    // vkGetDeviceQueue(
    //     m_vk_device,
    //     indices.present_family.value(),
    //     0,
    //     &m_vk_present_queue
    // );

    // 3. Create Vma Allocator //
    VmaAllocatorCreateInfo allocator_info {};
    allocator_info.physicalDevice = m_vk_physical_device;
    allocator_info.device = m_vk_device;
    allocator_info.instance = m_instance->vk_instance();
    auto result = vmaCreateAllocator(&allocator_info, &m_vma_allocator);
    VK_CHECK_MSG(result, "Failed to create Vma allocator");
}

Device::~Device() {
    // Destroy Vma Allocator
    vmaDestroyAllocator(m_vma_allocator);
    // Destroy logical device instance
    vkDestroyDevice(m_vk_device, nullptr);
    Log::trace("Device destroyed");
}

} // namespace kzn::vk