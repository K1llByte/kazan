#include "renderer.hpp"

#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <cstring>

namespace kzn
{

VkResult Renderer::CreateDebugUtilsMessenger(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
    const VkAllocationCallbacks*              p_allocator,
    VkDebugUtilsMessengerEXT*                 p_debug_messenger)
{
    // static_cast<PFN_vkCreateDebugUtilsMessengerEXT>()
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    return (func != nullptr)
        ? func(instance, p_create_info, p_allocator, p_debug_messenger)
        : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Renderer::DestroyDebugUtilsMessenger(
    VkInstance                   instance,
    VkDebugUtilsMessengerEXT     debug_messenger,
    const VkAllocationCallbacks* p_allocator)
{
    // static_cast<PFN_vkCreateDebugUtilsMessengerEXT>()
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debug_messenger, p_allocator);
}

void Renderer::run()
{
    init_window();
    init_vulkan();
    main_loop();
    cleanup();
}

void Renderer::init_window()
{
    // Initialize glfw
    glfwInit();
    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // Initialize window
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Kazan Vulkan", nullptr, nullptr);
}

void Renderer::init_vulkan()
{
    create_instance();
    setup_debug_messenger();
}

void Renderer::main_loop()
{
    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void Renderer::cleanup()
{
    if (ENABLE_VALIDATION_LAYERS)
        DestroyDebugUtilsMessenger(m_instance, m_debug_messenger, nullptr);

    // Destroy vulkan instance
    vkDestroyInstance(m_instance, nullptr);
    // Destroy window
    glfwDestroyWindow(m_window);
    // Destroy glfw
    glfwTerminate();
}

// =========== Create functions =========== //

void Renderer::create_instance()
{
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Vulkan";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Kazan";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    // ====== Extension loading ====== //

    // Enable required extensions
    auto req_extensions = get_required_extensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(req_extensions.size());
    create_info.ppEnabledExtensionNames = req_extensions.data();

    std::cout << "[INFO] > Enabled Extensions:\n";
    for (const auto& ext : req_extensions)
        std::cout << " - " << ext << '\n';

    // List supported extensions
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    std::cout << "[INFO] > Available Extensions:\n";
    for (const auto& ext : extensions)
        std::cout << " - " << ext.extensionName << '\n';
    
    // ====== Validation layers ====== //

    if (ENABLE_VALIDATION_LAYERS)
    {
        if(!check_validation_layer_support())
            throw std::runtime_error("validation layers requested, but not available!");

        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
    }
    else
    {
        // No validation layers
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    // Create instance
    if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}

bool Renderer::check_validation_layer_support()
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

std::vector<const char*> Renderer::get_required_extensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if(ENABLE_VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
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

void Renderer::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    create_info.sType =           VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType =     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr; // Optional
}

void Renderer::setup_debug_messenger()
{
    if(ENABLE_VALIDATION_LAYERS)
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info{};
        populate_debug_messenger_create_info(create_info);
        
        if(CreateDebugUtilsMessenger(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
            throw std::runtime_error("failed to set up debug messenger!");
    }
}

}