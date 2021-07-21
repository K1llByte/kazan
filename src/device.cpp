#include "device.hpp"

#include <iostream>
#include <algorithm>
//#include <iterator>

namespace kzn
{

    Device::Device(Window &window)
        : m_window{window}
    {
        create_instance();
        create_surface();
        pick_physical_device();
        create_logical_device();
    }

    Device::~Device()
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        // Physical device is automaticly destroyed
        // on instance destruction
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    SwapChainSupportDetails Device::get_swap_chain_support()
    {
        return query_swap_chain_support(m_physical_device);
    }

    QueueFamilyIndices Device::get_physical_queue_families()
    {
        return find_queue_families(m_physical_device);
    }

    void Device::create_instance()
    {
        if (enable_validation_layers && !has_validation_layers_support())
            throw std::runtime_error("validation layers requested, but not available!");

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        // appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Kazan";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto exts = get_required_extensions();
        createInfo.enabledExtensionCount = exts.size();
        createInfo.ppEnabledExtensionNames = exts.data();

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
            throw std::runtime_error("failed to create m_instance!");

        has_required_extensions();

        if (enable_validation_layers)
        {
            createInfo.enabledLayerCount = m_validation_layers.size();
            createInfo.ppEnabledLayerNames = m_validation_layers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }
    }

    std::vector<const char *> Device::get_required_extensions()
    {
        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;

        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        // if (enable_validation_layers)
        //     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    bool Device::has_required_extensions()
    {
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

        return true;
    }

    bool Device::has_validation_layers_support()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char *layer_name : m_validation_layers)
        {
            bool layer_found = false;

            for (const auto &layerProperties : available_layers)
            {
                if (strcmp(layer_name, layerProperties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
                return false;
        }

        return true;
    }

    void Device::pick_physical_device()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

        if (device_count == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

        for (const auto &d : devices)
        {
            if (is_device_suitable(d))
            {
                m_physical_device = d;
                break;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE)
            throw std::runtime_error("failed to find a suitable GPU!");
    }

    bool Device::is_device_suitable(VkPhysicalDevice device)
    {
#ifdef REQUIRE_DEDICATED_GPU
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);

        // VkPhysicalDeviceFeatures device_features;
        // vkGetPhysicalDeviceFeatures(device, &device_features);

        return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
#else
        QueueFamilyIndices indices = find_queue_families(device);

        if(has_device_extensions_support(device))
        {
            SwapChainSupportDetails swap_chain_support = query_swap_chain_support(device);
            bool swap_chain_is_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();

            return indices.is_complete() && swap_chain_is_adequate;
        }

        return false;
#endif
    }

    bool Device::has_device_extensions_support(VkPhysicalDevice device)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        for (const auto &ext : m_device_extensions)
            for (const auto &avaiable : available_extensions)
                if (avaiable.extensionName == ext)
                    return false;

        return true;
    }

    QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        // Logic to find queue family indices to populate struct with
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        int i = 0;
        for (const auto &queue_family : queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphics_family = i;

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.present_family = i;

            if (indices.is_complete())
                break;

            i++;
        }

        return indices;
    }

    void Device::create_logical_device()
    {
        // FIXME: 'find_queue_families' is called
        // too many times, which is unecessary.
        QueueFamilyIndices indices = find_queue_families(m_physical_device);

        // VkDeviceQueueCreateInfo queue_create_info{};
        // queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // queue_create_info.queueFamilyIndex = indices.graphics_family.value();
        // queue_create_info.queueCount = 1;

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info = {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queueFamily;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queuePriority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.queueCreateInfoCount = queue_create_infos.size();

        create_info.pEnabledFeatures = &device_features;

        create_info.enabledExtensionCount = m_device_extensions.size();
        create_info.ppEnabledExtensionNames = m_device_extensions.data();

        // Device specific validation layers are
        // deprecated but this is here to ensure
        // compatibility with older versions
        if (enable_validation_layers)
        {
            create_info.enabledLayerCount = m_validation_layers.size();
            create_info.ppEnabledLayerNames = m_validation_layers.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
    }

    void Device::create_surface()
    {
        m_window.create_window_surface(m_instance, &m_surface);
    }

    SwapChainSupportDetails Device::query_swap_chain_support(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);

        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);

        if (present_mode_count != 0)
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, details.present_modes.data());
        }

        return details;
    }
}