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

    if(func != nullptr)
    {
        std::cout << "+ Debug messeger created successfully\n";
        return func(instance, p_create_info, p_allocator, p_debug_messenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // return (func != nullptr)
    //     ? func(instance, p_create_info, p_allocator, p_debug_messenger)
    //     : VK_ERROR_EXTENSION_NOT_PRESENT;
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
    {
        std::cout << "- Debug messeger destroyed successfully\n";
        func(instance, debug_messenger, p_allocator);
    }
    
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


Instance::Instance()
{
    _tmp_data = new TemporaryData();

    _tmp_data->_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    _tmp_data->_app_info.pApplicationName = "Hello Kazan";
    _tmp_data->_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    _tmp_data->_app_info.pEngineName = "Kazan";
    _tmp_data->_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    _tmp_data->_app_info.apiVersion = VK_API_VERSION_1_0;

    _tmp_data->_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _tmp_data->_create_info.pApplicationInfo = &_tmp_data->_app_info;

    // By default validation layers won't come
    // enabled
    _tmp_data->_create_info.enabledLayerCount = 0;
    _tmp_data->_create_info.pNext = nullptr;
}


Instance::~Instance()
{
    cleanup();
}


Instance& Instance::set_debug_messeger()
{
    if(_enable_validation_layers)
    {
        _extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        _enable_debug_messeger = true;

        //VkDebugUtilsMessengerCreateInfoEXT _tmp_data->_debug_create_info{};
        _tmp_data->_debug_create_info.sType =           VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        _tmp_data->_debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                    ;
        _tmp_data->_debug_create_info.messageType =     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                    ;
        _tmp_data->_debug_create_info.pfnUserCallback = debug_callback;
        _tmp_data->_debug_create_info.pUserData = nullptr; // Optional
    }

    return *this;
}

Instance& Instance::enable_extensions(const std::vector<const char*>& extensions)
{
    _extensions.insert(
        _extensions.end(),
        extensions.begin(),
        extensions.end()
        );
    return *this;
}

// InstanceBuilder& InstanceBuilder::enable_extension(const char* extension)
// {
//     _extensions.push_back(extensions);
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
    for (const char* layer_name : validation_layers)
    {
        bool layer_found = false;

        for (const auto& layer_properties : available_layers)
        {
            if (strcmp(layer_name, layer_properties.layerName) == 0)
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

Instance& Instance::enable_validation_layers()
{
    _enable_validation_layers = true;
    _validation_layers.push_back("VK_LAYER_KHRONOS_validation");

    if(!check_validation_layers_support(_validation_layers))
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    _tmp_data->_create_info.enabledLayerCount = static_cast<uint32_t>(_validation_layers.size());
    _tmp_data->_create_info.ppEnabledLayerNames = _validation_layers.data();

    return *this;
}

void Instance::init()
{
    _tmp_data->_create_info.enabledExtensionCount = static_cast<uint32_t>(_extensions.size());
    _tmp_data->_create_info.ppEnabledExtensionNames = _extensions.data();

    ///////////////// LOGS /////////////////
    // std::cout << "[INFO] > Enabled Extensions:\n";
    // for (const auto& ext : _extensions)
    //     std::cout << " - " << ext << '\n';
    ////////////////////////////////////////

    // Create instance
    std::cout << "+ Instance created successfully\n";
    if(vkCreateInstance(&_tmp_data->_create_info, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    // Setup debug callback
    if(_enable_debug_messeger)
    {
        VkResult res = CreateDebugUtilsMessenger(_instance, &_tmp_data->_debug_create_info, nullptr, &_debug_messenger);
        if(res != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    delete _tmp_data;
}


void Instance::cleanup()
{
    // Destroy debug messeger
    if(_enable_debug_messeger)
    {
        DestroyDebugUtilsMessenger(_instance, _debug_messenger, nullptr);
    }

    // Destroy vulkan instance
    vkDestroyInstance(_instance, nullptr);
    std::cout << "- Destroyed vulkan instance\n";
}


VkInstance Instance::instance() const
{
    return _instance;
}


bool Instance::initialized() const
{
    return _tmp_data != nullptr;
}

} // namespace kzn