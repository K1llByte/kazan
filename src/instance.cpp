#include "instance.hpp"

#include <stdexcept>
#include <cstring>

#include <iostream>

namespace kzn
{

VkResult CreateDebugUtilsMessenger(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
    const VkAllocationCallbacks*              p_allocator,
    VkDebugUtilsMessengerEXT*                 p_debug_messenger)
{
    // TODO:
    // static_cast<PFN_vkCreateDebugUtilsMessengerEXT>()
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    return (func != nullptr)
        ? func(instance, p_create_info, p_allocator, p_debug_messenger)
        : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessenger(
    VkInstance                   instance,
    VkDebugUtilsMessengerEXT     debug_messenger,
    const VkAllocationCallbacks* p_allocator)
{
    // TODO:
    // static_cast<PFN_vkCreateDebugUtilsMessengerEXT>()
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
        func(instance, debug_messenger, p_allocator);
    
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      /* message_severity */,
    VkDebugUtilsMessageTypeFlagsEXT             /* message_type */,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
    void*                                       /* p_user_data */)
{
    std::cerr << "[VALIDATION] > " << p_callback_data->pMessage << std::endl;
    return VK_FALSE;
}

Instance::Instance(VkInstance instance, bool enable_debug_messeger, VkDebugUtilsMessengerEXT debug_messenger)
    : m_instance{instance}, 
    m_enable_debug_messeger{enable_debug_messeger},
    m_debug_messenger{debug_messenger}
{

}

Instance::~Instance()
{
    std::cout << "~Instance\n";
    if (m_enable_debug_messeger)
    {
        DestroyDebugUtilsMessenger(m_instance, m_debug_messenger, nullptr);
        std::cout << "~Instance::DestroyDebugUtilsMessenger\n";
    }

    // Destroy vulkan instance
    vkDestroyInstance(m_instance, nullptr);
}

InstanceBuilder::InstanceBuilder()
{
    m_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    m_app_info.pApplicationName = "Hello Vulkan";
    m_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    m_app_info.pEngineName = "Kazan";
    m_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    m_app_info.apiVersion = VK_API_VERSION_1_0;

    m_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    m_create_info.pApplicationInfo = &m_app_info;

    // By default validation layers won't come
    // enabled
    m_create_info.enabledLayerCount = 0;
    m_create_info.pNext = nullptr;

    // // List supported extensions
    // uint32_t extension_count = 0;
    // vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    // std::vector<VkExtensionProperties> extensions(extension_count);
    // vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());


    // if(ENABLE_VALIDATION_LAYERS)
    // {
    //     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    // }
}

InstanceBuilder& InstanceBuilder::set_debug_messeger()
{
    if(m_enable_validation_layers)
    {
        m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        m_enable_debug_messeger = true;
        //VkDebugUtilsMessengerCreateInfoEXT m_debug_create_info{};
        m_debug_create_info.sType =           VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        m_debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                    ;
        m_debug_create_info.messageType =     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                    ;
        m_debug_create_info.pfnUserCallback = debug_callback;
        m_debug_create_info.pUserData = nullptr; // Optional
    }

    return *this;
}

InstanceBuilder& InstanceBuilder::enable_extensions(const std::vector<const char*>& extensions)
{
    m_extensions.insert(
        m_extensions.end(),
        extensions.begin(),
        extensions.end()
        );
    return *this;
}

// InstanceBuilder& InstanceBuilder::enable_extension(const char* extension)
// {
//     m_extensions.push_back(extensions);
//     return *this;
// }

bool check_validation_layers_support(const std::vector<const char*>& validation_layers)
{
    // Retrieve avaiable validation layers
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    // Check for each wanted validation layers
    // that they exist in the available layers
    // vector
    for (const char* layerName : validation_layers)
    {
        bool layer_found = false;

        for (const auto& layerProperties : available_layers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
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

InstanceBuilder& InstanceBuilder::enable_validation_layers()
{
    m_enable_validation_layers = true;
    m_validation_layers.push_back("VK_LAYER_KHRONOS_validation");

    if(!check_validation_layers_support(m_validation_layers))
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    m_create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
    m_create_info.ppEnabledLayerNames = m_validation_layers.data();

    return *this;
}

Instance InstanceBuilder::build()
{
    m_create_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
    m_create_info.ppEnabledExtensionNames = m_extensions.data();

    ///////////////// LOGS /////////////////
    // std::cout << "[INFO] > Enabled Extensions:\n";
    // for (const auto& ext : m_extensions)
    //     std::cout << " - " << ext << '\n';
    ////////////////////////////////////////

    VkInstance vkinstance;

    // Create instance
    if(vkCreateInstance(&m_create_info, nullptr, &vkinstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    // Setup debug callback
    if(m_enable_debug_messeger)
    {
        VkResult res = CreateDebugUtilsMessenger(vkinstance, &m_debug_create_info, nullptr, &m_debug_messenger);
        if(res != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    return Instance(vkinstance, m_enable_debug_messeger);;
}

} // namespace kzn