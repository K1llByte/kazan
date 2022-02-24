#include "vk/device.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"
#include "vk/utils.hpp"

#include <unordered_set>

namespace kzn::vk
{
    Device::~Device()
    {
        // Destroy logical device instance
        vkDestroyDevice(vkdevice, nullptr);
        Log::debug("Device destroyed");
    }

    QueueFamilyIndices DeviceBuilder::get_queue_families(VkPhysicalDevice physical_device)
    {

        // Logic to find queue family indices to populate struct with
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

        QueueFamilyIndices indices;
        for(size_t i = 0 ; i < queue_families.size() ; ++i)
        {
            if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }
            // VkBool32 present_support = false;
            // vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
            // if(present_support)
            //     indices.present_family = i;
            if(indices.is_complete())
                break;
        }

        return indices;
    }

    DeviceBuilder::DeviceBuilder(Instance& instance)
    {
        auto vkinstance = instance.vk_instance();
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vkinstance, &device_count, nullptr);
        if(device_count == 0)
        {
            throw NoGPUSupport();
        }
        available_devices.resize(device_count);
        vkEnumeratePhysicalDevices(vkinstance, &device_count, available_devices.data());
    }

    Device DeviceBuilder::build()
    {
        // 1. Select physical device.
        // 2. Create device
        //   2.1. Create device queues
        //   2.2. Create cmd pool

        // 1. Select physical device //
        // For each available device check if is suitable.
        VkPhysicalDevice vkphysical_device = VK_NULL_HANDLE;
        QueueFamilyIndices indices;
        for (const auto& device : available_devices)
        {
            bool is_device_suitable = false;

            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(device, &device_properties);
            VkPhysicalDeviceFeatures device_features;
            vkGetPhysicalDeviceFeatures(device, &device_features);
            // Require to be a dedicated GPU
            if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                indices = get_queue_families(device);
                // bool extensions_supported = check_device_extension_support(physical_device);
                // bool swap_chain_adequate = false;
                // if(extensions_supported)
                // {
                //     swap_chain_support = query_swap_chain_support(physical_device);
                //     swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
                // }
                // return _indices.is_complete() && swap_chain_adequate && device_features.samplerAnisotropy;
                is_device_suitable = indices.is_complete(); //&& extensions_supported;
            }

            if(is_device_suitable)
            {
                vkphysical_device = device;
                break;
            }
        }

        if(vkphysical_device == VK_NULL_HANDLE)
        {
            throw NoGPUSuitable();
        }
        else
        {
            // Log::debug("Physical device selected successfully");
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(vkphysical_device, &device_properties);
            Log::info("Selected GPU: {}", device_properties.deviceName);
        }

        // 2. Create device //
        // TODO: Refactor this, there's no need to create
        // an auxiliar structure, elaborate the
        // QueueFamilyIndices class to provide an iterator 
        std::unordered_set<uint32_t> unique_queue_families{
            indices.graphics_family.value(),
            // indices.present_family.value()
        };

        float queue_priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        for(uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        // TODO: Setter for all device features
        // one setter to rule them all
        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;

        // TODO: Fetch this from vk::Instance instead of hardcoded 
        std::vector<const char*> validation_layers{"VK_LAYER_KHRONOS_validation"};
        std::vector<const char*> device_extensions{}; // {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pEnabledFeatures = &device_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

        VkDevice vkdevice = VK_NULL_HANDLE;
        auto result = vkCreateDevice(vkphysical_device, &create_info, nullptr, &vkdevice);
        VK_CHECK_MSG(result, "Failed to create logical device!");
        Log::debug("Device created");

        // 2.1. Create device queues //
        vkGetDeviceQueue(vkdevice, indices.graphics_family.value(), 0, &graphics_queue);
        // vkGetDeviceQueue(_device, indices.present_family.value(), 0, &present_queue);

        // // 2.2. Create cmd pool //
        // // TODO: Consider moving this to a separate method in Device
        // // like create_cmd_pool
        // VkCommandPoolCreateInfo pool_info{};
        // pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        // pool_info.queueFamilyIndex = indices.graphics_family.value();
        // pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
        //                 | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        // VkCommandPool command_pool;
        // result = vkCreateCommandPool(vkdevice, &pool_info, nullptr, &command_pool);
        // VK_CHECK_MSG(result, "Failed to create command pool!");

        auto device = Device();
        device.vkdevice = vkdevice;
        return device;
    }
} // namespace kzn::vk