#include "vk/instance.hpp"

#include "vk/utils.hpp"
#include "vk/error.hpp"
#include "config.hpp"

#include <array>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      /*message_severity*/,
    VkDebugUtilsMessageTypeFlagsEXT             /*message_type*/,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
    void*                                       /*p_user_data*/)
{
    kzn::Log::warning(p_callback_data->pMessage);
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
    const VkAllocationCallbacks* p_allocator,
    VkDebugUtilsMessengerEXT* p_debug_messenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );
    if(func != nullptr)
    {
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* p_allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debug_messenger, p_allocator);
    }
}

namespace kzn::vk
{
    Instance::~Instance()
    {
        if(with_validation_layers)
        {
            DestroyDebugUtilsMessengerEXT(vkinstance, debug_messenger, nullptr);
        }
        if(surface != VK_NULL_HANDLE)
        {
            // Destroy surface
            vkDestroySurfaceKHR(vkinstance, surface, nullptr);
            Log::debug("Surface destroyed");
        }
        vkDestroyInstance(vkinstance, nullptr);
        Log::debug("Instance destroyed");
    }

    const std::vector<const char*>& Instance::get_validation_layers() const noexcept
    {
        return validation_layers; 
    }

    // TODO: Change to Window
    VkSurfaceKHR Instance::create_surface(Window& window)
    {
        // Create surface
        auto result = glfwCreateWindowSurface(vkinstance, window.glfw_ptr(), nullptr, &surface);
        VK_CHECK_MSG(result, "Failed to create window surface!");
        Log::debug("Surface created");
        return surface;
    }

    InstanceBuilder& InstanceBuilder::enable_validation_layers() noexcept
    {
        with_validation_layers = true;
        return *this;
    }

    InstanceBuilder& InstanceBuilder::set_extensions(const std::vector<const char*>& extensions) noexcept
    {
        this->instance_extensions.insert(
            this->instance_extensions.end(),
            extensions.begin(),
            extensions.end()
            );
        return *this;
    }

    Instance InstanceBuilder::build()
    {
        // 1. Enable validation layers
        // 2. Enable extensions
        // 3. Create instance
        // 4. Setup debug messeger

        // Create info data is changed in 1.
        VkInstanceCreateInfo create_info{};
        VkInstance vkinstance = VK_NULL_HANDLE;

        // 1. Enable validation layers //
        if(with_validation_layers)
        {
            uint32_t layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
            std::vector<VkLayerProperties> available_layers(layer_count);
            vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

            bool has_validation_layers = true;
            for (const char* layerName : validation_layers)
            {
                bool layer_found = false;
                for(const auto& layerProperties : available_layers)
                {
                    if(strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        layer_found = true;
                        break;
                    }
                }

                if(!layer_found)
                {
                    has_validation_layers = false;
                }
            }

            if(!has_validation_layers)
            {
                Log::error("Validation Layers not available");
                throw FailedValidationLayers();
            }
            else
            {
                // Validation layers are supported
                create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
                create_info.ppEnabledLayerNames = validation_layers.data();

                instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
        }

        // 2. Enable extensions //
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());

        // Log::debug("Available extensions:");
        // for (const auto& extension : available_extensions)
        // {
        //     Log::debug(extension.extensionName);
        // }

        // 3. Enable validation layers //
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Unnamed";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Kazan";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion =  VK_API_VERSION_1_2; // VK_API_VERSION_1_0;

        // Extensions get loaded from outside
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size());
        create_info.ppEnabledExtensionNames = instance_extensions.data();
        Log::debug("Enabled extensions:");
        for (const auto& extension_name : instance_extensions)
        {
            Log::debug("- {}", extension_name);
        }

        auto result = vkCreateInstance(&create_info, nullptr, &vkinstance);
        VK_CHECK_MSG(result, "Failed to create VkInstance (VkResult = {})");
        Log::debug("Instance created");

        // 4. Setup debug messeger //
        if(with_validation_layers)
        {
            // FIXME: Consider moving this setup of the debug messeger to the
            // Instance method since it can be dynamicly changed
            VkDebugUtilsMessengerCreateInfoEXT messenger_create_info{};
            messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                        // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                        // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                        ;
            messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                    ;
            messenger_create_info.pfnUserCallback = debug_callback;
            messenger_create_info.pUserData = nullptr; // Optional
            
            if(CreateDebugUtilsMessengerEXT(vkinstance, &messenger_create_info, nullptr, &debug_messenger) != VK_SUCCESS)
            {
                throw FailedDebugMessenger();
            }
        }

        // TODO: This is being initialized by default ctor, thats no good
        Instance instance;
        instance.with_validation_layers = with_validation_layers;
        instance.debug_messenger = debug_messenger;
        instance.vkinstance = vkinstance;
        instance.validation_layers = std::move(validation_layers);
        return instance;
    }
} // namespace kzn::vk