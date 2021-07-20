// #include "test_app.hpp"

// int main()
// {
//     kzn::TestApp app{};
//     app.run();
//     return 0;
// }

/////////////////////////////////////////////////////

// Vulkan basic components
// - Vulkan instance
// - Physical device
// - Logical device
// - Swapchain

/////////////////////////////////////////////////////

#include "window.hpp"
#include "device.hpp"

// #include <vulkan/vulkan.h>

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

#include <iostream>
// #include <stdexcept>
// #include <vector>
// #include <set>
// #include <optional>
// #include <cstdlib>
// #include <cstring>

// struct QueueFamilyIndices
// {
//     std::optional<uint32_t> graphics_family;
//     std::optional<uint32_t> present_family;

//     bool is_complete()
//     {
//         return graphics_family.has_value() && present_family.has_value();
//     }
// };

// class Device
// {
// private:
//     kzn::Window& window;

//     VkInstance m_instance;
//     VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
//     VkDevice m_device;
//     VkQueue m_graphics_queue;
//     VkQueue m_present_queue;
//     VkSurfaceKHR m_surface;

//     const bool enable_validation_layers = false;
//     const std::vector<const char *> validation_layers = {
//         "VK_LAYER_KHRONOS_validation"};

// public:
//     Device(kzn::Window& in_window)
//         : window{in_window}
//     {
//         create_instance();
//         create_surface();
//         pick_physical_device();
//         create_logical_device();
//     }

//     ~Device()
//     {
//         vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
//         vkDestroyInstance(m_instance, nullptr);
//         // Physical device is automaticly destroyed
//         vkDestroyDevice(m_device, nullptr);
//     }

//     // Cannot be copyable or movable
//     Device(const Device &) = delete;
//     void operator=(const Device &) = delete;
//     Device(Device &&) = delete;
//     Device &operator=(Device &&) = delete;

//     void run()
//     {
//         main_loop();
//     }

// private:
//     void main_loop()
//     {
//     }

//     void create_instance()
//     {
//         if (enable_validation_layers && !has_validation_layers_support())
//             throw std::runtime_error("validation layers requested, but not available!");

//         VkApplicationInfo appInfo{};
//         appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//         // appInfo.pNext = nullptr;
//         appInfo.pApplicationName = "Hello Triangle";
//         appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//         appInfo.pEngineName = "Kazan";
//         appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//         appInfo.apiVersion = VK_API_VERSION_1_0;

//         VkInstanceCreateInfo createInfo{};
//         createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//         createInfo.pApplicationInfo = &appInfo;

//         auto exts = get_required_extensions();
//         createInfo.enabledExtensionCount = exts.size();
//         createInfo.ppEnabledExtensionNames = exts.data();

//         if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
//             throw std::runtime_error("failed to create m_instance!");

//         has_required_extensions();

//         if (enable_validation_layers)
//         {
//             createInfo.enabledLayerCount = validation_layers.size();
//             createInfo.ppEnabledLayerNames = validation_layers.data();
//         }
//         else
//         {
//             createInfo.enabledLayerCount = 0;
//         }
//     }

//     std::vector<const char *> get_required_extensions()
//     {
//         uint32_t glfw_extension_count = 0;
//         const char **glfw_extensions;

//         glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

//         std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

//         // if (enable_validation_layers)
//         //     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

//         return extensions;
//     }

//     bool has_required_extensions()
//     {
//         uint32_t extension_count = 0;
//         vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

//         std::vector<VkExtensionProperties> extensions(extension_count);
//         vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

//         return true;
//     }

//     bool has_validation_layers_support()
//     {
//         uint32_t layer_count;
//         vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

//         std::vector<VkLayerProperties> available_layers(layer_count);
//         vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

//         for (const char *layer_name : validation_layers)
//         {
//             bool layer_found = false;

//             for (const auto &layerProperties : available_layers)
//             {
//                 if (strcmp(layer_name, layerProperties.layerName) == 0)
//                 {
//                     layer_found = true;
//                     break;
//                 }
//             }

//             if (!layer_found)
//                 return false;
//         }

//         return true;
//     }

//     void pick_physical_device()
//     {
//         uint32_t device_count = 0;
//         vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

//         if (device_count == 0)
//             throw std::runtime_error("failed to find GPUs with Vulkan support!");

//         std::vector<VkPhysicalDevice> devices(device_count);
//         vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

//         for (const auto& d : devices)
//         {
//             if (is_device_suitable(d))
//             {
//                 m_physical_device = d;
//                 break;
//             }
//         }

//         if (m_physical_device == VK_NULL_HANDLE)
//             throw std::runtime_error("failed to find a suitable GPU!");
//     }

//     bool is_device_suitable(VkPhysicalDevice device)
//     {
// #ifdef REQUIRE_DEDICATED_GPU
//         VkPhysicalDeviceProperties device_properties;
//         vkGetPhysicalDeviceProperties(device, &device_properties);

//         // VkPhysicalDeviceFeatures device_features;
//         // vkGetPhysicalDeviceFeatures(device, &device_features);

//         return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
// #else
//         QueueFamilyIndices indices = find_queue_families(device);

//         return indices.graphics_family.has_value();
// #endif
//     }

//     QueueFamilyIndices find_queue_families(VkPhysicalDevice device)
//     {
//         QueueFamilyIndices indices;
//         // Logic to find queue family indices to populate struct with
//         uint32_t queue_family_count = 0;
//         vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

//         std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
//         vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

//         int i = 0;
//         for (const auto &queue_family : queue_families)
//         {
//             if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//                 indices.graphics_family = i;

//             VkBool32 present_support = false;
//             vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
//             if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//                 indices.present_family = i;

//             if (indices.is_complete())
//                 break;

//             i++;
//         }

//         return indices;
//     }

//     void create_logical_device()
//     {
//         // FIXME: 'find_queue_families' is called
//         // too many times, which is unecessary.
//         QueueFamilyIndices indices = find_queue_families(m_physical_device);

//         // VkDeviceQueueCreateInfo queue_create_info{};
//         // queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//         // queue_create_info.queueFamilyIndex = indices.graphics_family.value();
//         // queue_create_info.queueCount = 1;

//         std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
//         std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

//         float queuePriority = 1.0f;
//         for (uint32_t queueFamily : unique_queue_families)
//         {
//             VkDeviceQueueCreateInfo queue_create_info = {};
//             queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//             queue_create_info.queueFamilyIndex = queueFamily;
//             queue_create_info.queueCount = 1;
//             queue_create_info.pQueuePriorities = &queuePriority;
//             queue_create_infos.push_back(queue_create_info);
//         }

//         VkPhysicalDeviceFeatures device_features{};

//         VkDeviceCreateInfo create_info{};
//         create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

//         create_info.pQueueCreateInfos = queue_create_infos.data();
//         create_info.queueCreateInfoCount = queue_create_infos.size();

//         create_info.pEnabledFeatures = &device_features;

//         // Device specific validation layers are
//         // deprecated but this is here to ensure
//         // compatibility with older versions
//         create_info.enabledExtensionCount = 0;
//         if (enable_validation_layers)
//         {
//             create_info.enabledLayerCount = validation_layers.size();
//             create_info.ppEnabledLayerNames = validation_layers.data();
//         }
//         else
//         {
//             create_info.enabledLayerCount = 0;
//         }

//         if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
//         {
//             throw std::runtime_error("failed to create logical device!");
//         }

//         vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
//         vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
//     }

//     void create_surface()
//     {
//         window.create_window_surface(m_instance, &m_surface);
//     }
// };

/////////////////////////////////////////////////////

#include "window.hpp"

int main()
{
    kzn::Window window(800, 800, "Test Window");
    kzn::Device app(window);

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}