#include "vk/device.hpp"

#include "core/log.hpp"

namespace kzn::vk
{
    PhysicalDeviceSelector::PhysicalDeviceSelector(Instance& instance)
        : vkinstance(instance.vk_instance())
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vkinstance, &device_count, nullptr);

        if(device_count == 0)
        {
            // TODO: Wrap in PhysicalDeviceError
            std::__throw_runtime_error("Failed to find GPUs with Vulkan support!");
        }

        available_devices.resize(device_count);
        vkEnumeratePhysicalDevices(vkinstance, &device_count, available_devices.data());
    }

    PhysicalDevice PhysicalDeviceSelector::select()
    {
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        for (const auto& device : available_devices)
        {
            if(is_device_suitable(device))
            {
                physical_device = device;
                break;
            }
        }

        if(physical_device == VK_NULL_HANDLE)
        {
            // TODO: Wrap in PhysicalDeviceError
            std::__throw_runtime_error("Failed to find a suitable GPU!");
        }
        else
        {
            // Log::debug("Physical device selected successfully");
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(physical_device, &device_properties);

            Log::info("Selected GPU: {}", device_properties.deviceName);

            return PhysicalDevice{};
            // (
            //     physical_device,
            //     std::move(indices),
            //     std::move(device_extensions),
            //     std::move(swap_chain_support)
            //     );
        }
    }

    bool PhysicalDeviceSelector::is_device_suitable(VkPhysicalDevice physical_device)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);

        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(physical_device, &device_features);

        // Require to be a dedicated GPU
        if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return false;
        }

        /* QueueFamilyIndices indices = */
        find_queue_families(physical_device);

        // bool extensions_supported = check_device_extension_support(physical_device);

        // bool swap_chain_adequate = false;
        // if(extensions_supported)
        // {
        //     swap_chain_support = query_swap_chain_support(physical_device);
        //     swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        // }

        // return _indices.is_complete() && swap_chain_adequate && device_features.samplerAnisotropy;
        return indices.is_complete(); //&& extensions_supported;
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
} // namespace kzn::vk