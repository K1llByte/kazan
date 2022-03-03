#include "vk/device.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"
#include "vk/utils.hpp"

#include <unordered_set>
#include <algorithm>
#include <limits>

namespace kzn::vk
{
    QueueFamilyIndices get_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
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
            // The Present Queue support check isn't in an else statement
            // because there could be a queue_family with support for both
            // Graphics and Present
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
            if(present_support)
                indices.present_family = i;
            if(indices.is_complete())
                break;
        }

        return indices;
    }

    bool check_device_extensions_support(VkPhysicalDevice physical_device, const std::vector<const char*>& device_extensions)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

        // TODO: Change set creation to faster check
        std::unordered_set<std::string> required_extensions(
            device_extensions.begin(),
            device_extensions.end()
            );
        for (const auto& extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }
        return required_extensions.empty();
    }

    SwapChainSupport get_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
    {
        SwapChainSupport support;

        // 1. Get Surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &support.capabilities);

        // 2. Get Surface possible formats
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
        if(format_count != 0)
        {
            support.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, support.formats.data());
        }

        // 3. Get Surface possible present modes
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
        if(present_mode_count != 0)
        {
            support.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, support.present_modes.data());
        }

        return support;
    }

    VkSurfaceFormatKHR SwapChainSupport::select_format() const noexcept
    {
        for(const auto& format : formats)
        {
            if(format.format == VK_FORMAT_B8G8R8A8_SRGB
                && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }
        // Possible seg fault (not if its garanteed that the
        // formats list has at least one value)
        return formats[0];
    }

    VkPresentModeKHR SwapChainSupport::select_present_mode() const noexcept
    {
        for(const auto& present_mode : present_modes)
        {
            if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return present_mode;
            }
        }
        // FIFO mode is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChainSupport::select_extent(VkExtent2D extent) const noexcept
    {
        // Window managers set currentExtent width and height when ...
        // 
        if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            // Move this to window and pass it as argument
            int width, height;
            // glfwGetFramebufferSize(window, &width, &height);
            VkExtent2D extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
            extent.width = std::clamp(
                extent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
            extent.height = std::clamp(
                extent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);
            return extent;
        }
    }

    Device::~Device()
    {
        // Destroy logical device instance
        vkDestroyDevice(vkdevice, nullptr);
        Log::debug("Device destroyed");
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

        // Copy validation layer list
        validation_layers = instance.get_validation_layers();
    }

    DeviceBuilder& DeviceBuilder::set_surface(VkSurfaceKHR surface)
    {
        // NOTE: Device creation with Present Queue requires
        // surface handle before creation
        this->surface = surface;
        return *this;
    }

    DeviceBuilder& DeviceBuilder::set_extensions(const std::vector<const char*>& extensions)
    {
        this->device_extensions.insert(
            this->device_extensions.end(),
            extensions.begin(),
            extensions.end()
            );
        return *this;
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
        SwapChainSupport swapchain_support;
        for(const auto& iter_device : available_devices)
        {
            // Loop body to check if its a suitable device
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(iter_device, &device_properties);
            VkPhysicalDeviceFeatures device_features;
            vkGetPhysicalDeviceFeatures(iter_device, &device_features);

            // TODO: This if statements are going too diagonal, maybe make it more vertical
            // Fix: if(!condition) { continue; }
            // NOTE: Serveral if statements are being made so that the
            // work of the next checks isnt being done for no reason

            // 1. Require to be a dedicated GPU
            if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                continue;

            indices = get_queue_families(iter_device, surface);
            // 2. Require to have all needed queue families
            if(!indices.is_complete())
                continue;

            // 3. Require to support required device extensions
            if(!check_device_extensions_support(iter_device, device_extensions))
                continue;
            
            swapchain_support = get_swapchain_support(iter_device, surface);
            // 4. Require SwapChain support
            if(swapchain_support.formats.empty() || swapchain_support.present_modes.empty())
                continue;
            
            // This device is suitable, end loop!
            vkphysical_device = iter_device;
            break;
        }

        if(vkphysical_device == VK_NULL_HANDLE)
        {
            throw NoGPUSuitable();
        }
        else
        {
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
            indices.present_family.value()
        };

        float queue_priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos(unique_queue_families.size());
        size_t i = 0;
        for(uint32_t queue_family : unique_queue_families)
        {
            queue_create_infos[i] = VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queue_family,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
            };
            ++i;
        }

        // TODO: Setter for all device features
        // one setter to rule them all
        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;

        // TODO: Fetch this from Instance instead of hardcoded 
        // std::vector<const char*> validation_layers{"VK_LAYER_KHRONOS_validation"};
        Log::debug("Device Validation Layers:");
        for(const auto& vl : validation_layers)
        {
            Log::debug("- {}", vl);
        }
        // TODO: Setters for device extensions
        // std::vector<const char*> device_extensions{}; // {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
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
        vkGetDeviceQueue(vkdevice, indices.present_family.value(), 0, &present_queue);

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
        device.swapchain_support = std::move(swapchain_support);
        device.queue_family_indices = std::move(indices);
        return device;
    }
} // namespace kzn::vk