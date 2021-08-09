#include "full_renderer.hpp"

#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <set>
#include <algorithm>

namespace tmp
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
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Initialize window
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Kazan Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);
    
    // On framebuffer resize callback
    glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
}

void Renderer::init_vulkan()
{
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_graphics_pipeline();
    create_framebuffers();
    create_command_pool();
    create_command_buffers();
    create_sync_objects();
}

void Renderer::main_loop()
{
    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        draw_frame();
    }

    vkDeviceWaitIdle(m_device);
}

void Renderer::draw_frame()
{
    vkWaitForFences(m_device, 1, &m_in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

    // 1. Acquire an image from the swap chain
    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(m_device, m_swap_chain, UINT64_MAX, m_image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swap_chain();
        return;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    // Check if a previous frame is using this image
    // there is its fence to wait on
    if (m_images_in_flight[image_index] != VK_NULL_HANDLE)
    {
        vkWaitForFences(m_device, 1, &m_images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    m_images_in_flight[image_index] = m_in_flight_fences[current_frame];

    // 2. Execute the command buffer
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { m_image_available_semaphores[current_frame] };
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[image_index];

    VkSemaphore signal_semaphores[] = { m_render_finished_semaphores[current_frame] };
    
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(m_device, 1, &m_in_flight_fences[current_frame]);

    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fences[current_frame]) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    // 3. Return the image to the swap chain for presentation
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    // VkSwapchainKHR swap_chains[] = { m_swap_chain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swap_chain; // swap_chains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr; // Optional

    // vkQueuePresentKHR(m_present_queue, &present_info);

    
    result = vkQueuePresentKHR(m_present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebuffer_resized)
    {
        m_framebuffer_resized = false;
        recreate_swap_chain();
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("failed to present swap chain image!");
    
    // vkQueueWaitIdle(m_present_queue);

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::cleanup()
{
    if (ENABLE_VALIDATION_LAYERS)
        DestroyDebugUtilsMessenger(m_instance, m_debug_messenger, nullptr);

    cleanup_swap_chain();

    // Destroy semaphores
    for(size_t i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; ++i)
    {
        vkDestroySemaphore(m_device, m_render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(m_device, m_image_available_semaphores[i], nullptr);
        vkDestroyFence(m_device, m_in_flight_fences[i], nullptr);
    }

    // Destroy command pool
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);

    // Destroy logical device instance
    vkDestroyDevice(m_device, nullptr);

    // Destroy window surface
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    // Destroy vulkan instance
    vkDestroyInstance(m_instance, nullptr);

    // Destroy window
    glfwDestroyWindow(m_window);

    // Destroy glfw
    glfwTerminate();
}

void Renderer::framebuffer_resize_callback(GLFWwindow* window, int /* width */, int /* height */)
{
    auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    app->m_framebuffer_resized = true;
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

        // // Commented because its showing an unusual error message
        // VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        // populate_debug_messenger_create_info(debug_create_info);
        // create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
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
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                ;
    create_info.messageType =     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                ;
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

void Renderer::pick_physical_device()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

    if (device_count == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

    std::cout << "[INFO] > Available Physicals Devices:\n";
    for(const auto& device : devices)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        std::cout << " - " << device_properties.deviceName << "\n";
    }

    for (const auto& device : devices)
    {
        if (is_device_suitable(device))
        {
            m_physical_device = device;
            break;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool Renderer::is_device_suitable(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    // VkPhysicalDeviceFeatures device_features;
    // vkGetPhysicalDeviceFeatures(device, &device_features);

    if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        return false;

    QueueFamilyIndices indices = find_queue_families(physical_device);

    bool extensions_supported = check_device_extension_support(physical_device);

    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = query_swap_chain_support(physical_device);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }

    return indices.is_complete() && swap_chain_adequate; // && extensions_supported
}

QueueFamilyIndices Renderer::find_queue_families(VkPhysicalDevice physical_device)
{
    QueueFamilyIndices indices;

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

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_support);
        if(present_support)
            indices.present_family = i;

        if(indices.is_complete())
            break;
    }

    return indices;
}

void Renderer::create_logical_device()
{
    QueueFamilyIndices indices = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };
    
    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());;

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    if(ENABLE_VALIDATION_LAYERS)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_present_queue);
}

void Renderer::create_surface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

}

bool Renderer::check_device_extension_support(VkPhysicalDevice physical_device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

SwapChainSupportDetails Renderer::query_swap_chain_support(VkPhysicalDevice physical_device)
{
    SwapChainSupportDetails details;

    // 1. VkSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &details.capabilities);

    // 2. VkSurfaceFormatKHR's
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, details.formats.data());
    }

    // 3. VkPresentModeKHR's
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR Renderer::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    for (const auto& available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }

    return available_formats[0];
}

VkPresentModeKHR Renderer::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
    for (const auto& available_present_mode : available_present_modes)
    {
        if(available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actual_extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actual_extent;
    }
}

void Renderer::create_swap_chain()
{
    SwapChainSupportDetails swap_chain_support = query_swap_chain_support(m_physical_device);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
    VkExtent2D extent = choose_swap_extent(swap_chain_support.capabilities);

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

    if(swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
    {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_surface;

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = find_queue_families(m_physical_device);
    uint32_t queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0; // Optional
        create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    
    create_info.preTransform = swap_chain_support.capabilities.currentTransform;

    // INFO: if we want window to be transparent
    // this needs to change
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
    m_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

    m_swap_chain_image_format = surface_format.format;
    m_swap_chain_extent = extent;
}

void Renderer::cleanup_swap_chain()
{
    // Destroy framebuffers
    for(auto framebuffer : m_swap_chain_framebuffers)
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);

    // Free command buffers
    vkFreeCommandBuffers(m_device, m_command_pool, static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());

    // Destroy pipeline layout
    vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);

    // Destroy pipeline layout
    vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);

    // Destroy render pass
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);

    for(auto image_view : m_swap_chain_image_views)
        vkDestroyImageView(m_device, image_view, nullptr);

    //  Destroy swap chain
    vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
}

void Renderer::create_image_views()
{
    m_swap_chain_image_views.resize(m_swap_chain_images.size());

    for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
    {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = m_swap_chain_images[i];

        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_swap_chain_image_format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &create_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}

void Renderer::create_framebuffers()
{
    m_swap_chain_framebuffers.resize(m_swap_chain_image_views.size());

    for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
        VkImageView attachments[] = {
            m_swap_chain_image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = m_swap_chain_extent.width;
        framebuffer_info.height = m_swap_chain_extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Renderer::recreate_swap_chain()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device);

    cleanup_swap_chain();

    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_graphics_pipeline();
    create_framebuffers();
    create_command_buffers();
}

std::vector<char> Renderer::read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("failed to open file!");
    
    size_t file_size = static_cast<size_t>(file.tellg()); // (size_t) 
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    return buffer;
}

void Renderer::create_graphics_pipeline()
{
    auto vert_shader_code = read_file("shaders/shader.vert.spv");
    auto frag_shader_code = read_file("shaders/shader.frag.spv");

    VkShaderModule vert_shader_module = create_shader_module(vert_shader_code);
    VkShaderModule frag_shader_module = create_shader_module(frag_shader_code);

    // Programable pipeline stages

    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = frag_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_shader_stage_info,
        frag_shader_stage_info
    };

    // Fixed pipeline stages

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swap_chain_extent.width);
    viewport.height = static_cast<float>(m_swap_chain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swap_chain_extent;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    // Alternatively:
    // VK_POLYGON_MODE_FILL
    // VK_POLYGON_MODE_LINE
    // VK_POLYGON_MODE_POINT

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // Per framebuffer struct
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                          | VK_COLOR_COMPONENT_G_BIT
                                          | VK_COLOR_COMPONENT_B_BIT
                                          | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    // color_blend_attachment.blendEnable = VK_TRUE;
    // color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    // color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    // color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    // // Unused stage
    // VkDynamicState dynamic_states[] = {
    //     VK_DYNAMIC_STATE_VIEWPORT,
    //     VK_DYNAMIC_STATE_LINE_WIDTH
    // };

    // // TODO:: Change to std::vector
    // // std::vector<VkDynamicState> dynamic_states{ 
    // //     VK_DYNAMIC_STATE_VIEWPORT,
    // //     VK_DYNAMIC_STATE_LINE_WIDTH
    // // };

    // VkPipelineDynamicStateCreateInfo dynamic_state{};
    // dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // dynamic_state.dynamicStateCount = 2;
    // dynamic_state.pDynamicStates = dynamic_states;

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // Optional
    pipeline_layout_info.pSetLayouts = nullptr; // Optional
    pipeline_layout_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout!");

    // ===== Create graphics pipeline ===== //

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;

    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr; // Optional
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr; // &dynamic_state; // Optional

    pipeline_info.layout = m_pipeline_layout;

    pipeline_info.renderPass = m_render_pass;
    pipeline_info.subpass = 0;

    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline!");

    vkDestroyShaderModule(m_device, frag_shader_module, nullptr);
    vkDestroyShaderModule(m_device, vert_shader_module, nullptr);
}

VkShaderModule Renderer::create_shader_module(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module;
    if (vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");
    
    return shader_module;
}

void Renderer::create_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_swap_chain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // TODO: Study subpasses
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass!");

}

void Renderer::create_command_pool()
{
    QueueFamilyIndices queue_family_indices = find_queue_families(m_physical_device);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    pool_info.flags = 0; // Optional

    if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void Renderer::create_command_buffers()
{
    // Create command buffers

    m_command_buffers.resize(m_swap_chain_framebuffers.size());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

    if (vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    // Record command buffers

    for (size_t i = 0; i < m_command_buffers.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_command_buffers[i], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");
        

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = m_render_pass;
        render_pass_info.framebuffer = m_swap_chain_framebuffers[i];

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = m_swap_chain_extent;

        VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

        vkCmdDraw(m_command_buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(m_command_buffers[i]);

        if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
    }
}

void Renderer::create_sync_objects()
{
    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    m_images_in_flight.resize(m_swap_chain_images.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0 ; i < MAX_FRAMES_IN_FLIGHT ; ++i)
    {
        if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

}