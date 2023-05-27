#include "vk/instance.hpp"

#include "core/log.hpp"
#include "vk/error.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fmt/core.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT /*message_severity*/,
    VkDebugUtilsMessageTypeFlagsEXT /*message_type*/,
    VkDebugUtilsMessengerCallbackDataEXT const* p_callback_data,
    void* /*p_user_data*/
) {
    kzn::Log::warning(p_callback_data->pMessage);
    return VK_FALSE;
}

VkResult create_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT const* p_create_info,
    VkAllocationCallbacks const* p_allocator,
    VkDebugUtilsMessengerEXT* p_debug_messenger
) {
    auto const func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );
    return (func != nullptr)
             ? func(instance, p_create_info, p_allocator, p_debug_messenger)
             : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroy_debug_utils_messenger_ext(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    VkAllocationCallbacks const* p_allocator
) {
    auto const func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
    );
    if (func != nullptr) {
        func(instance, debug_messenger, p_allocator);
    }
}

namespace kzn::vk {

Instance::Instance(InstanceParams&& params) {
    // 1. Enable validation layers
    // 2. Enable extensions
    // 3. Create instance
    // 4. Setup debug messeger

    // 1. Enable validation layers //
    if (params.with_validation) {
        // Get available layers
        std::uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(
            &layer_count,
            available_layers.data()
        );

        if (params.with_validation) {
            params.layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        for (auto const& layer : params.layers) {
            // Look for requested layer in available ones
            auto layers_it = std::ranges::find_if(
                available_layers,
                [layer](auto const lp) {
                    return std::strcmp(layer, lp.layerName) == 0;
                }
            );

            // If not found throw error
            if (layers_it == available_layers.end()) {
                throw std::invalid_argument(
                    "Requested validation Layers not available"
                );
            }
        }

        params.extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // 2. Enable extensions //
    std::uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extension_count,
        available_extensions.data()
    );

    // 3. Enable validation layers //
    VkApplicationInfo app_info {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Unnamed";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Kazan";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    // Extensions get loaded from outside
    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = static_cast<uint32_t>(params.layers.size());
    create_info.ppEnabledLayerNames = params.layers.data();
    create_info.enabledExtensionCount
        = static_cast<uint32_t>(params.extensions.size());
    create_info.ppEnabledExtensionNames = params.extensions.data();

    if (! params.extensions.empty()) {
        Log::trace("Enabled extensions:");
        for (auto ext_name : params.extensions) {
            Log::trace("- {}", ext_name);
        }
    }

    // Create VkInstance
    auto result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
    VK_CHECK_MSG(result, "Failed to create VkInstance (VkResult = {})");
    Log::trace("Instance created");

    // 4. Setup debug messeger //
    if (params.with_validation) {
        VkDebugUtilsMessengerCreateInfoEXT messenger_create_info {};
        messenger_create_info.sType
            = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messenger_create_info.messageSeverity
            = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        messenger_create_info.messageType
            = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        messenger_create_info.pfnUserCallback = debug_callback;

        result = create_debug_utils_messenger_ext(
            m_vk_instance,
            &messenger_create_info,
            nullptr,
            &m_debug_messenger
        );
        VK_CHECK_MSG(result, "Failed to create debug utils messenger");
    }
}

Instance::~Instance() {
    if (m_debug_messenger != VK_NULL_HANDLE) {
        destroy_debug_utils_messenger_ext(
            m_vk_instance,
            m_debug_messenger,
            nullptr
        );
    }

    // if (surface != VK_NULL_HANDLE)
    // {
    //     // Destroy surface
    //     vkDestroySurfaceKHR(vkinstance, surface, nullptr);
    //     // Log::trace("Surface destroyed");
    // }
    vkDestroyInstance(m_vk_instance, nullptr);
    Log::trace("Instance destroyed");
}

} // namespace kzn::vk