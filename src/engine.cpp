#include "engine.hpp"

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

// #include "types.hpp"
#include "initializers.hpp"

#include "VkBootstrap.h"

#include <iostream>

#define VK_CHECK(x)                                            \
    do                                                         \
    {                                                          \
        VkResult err = x;                                      \
        if (err)                                               \
        {                                                      \
            std::cout << "Vulkan Error: " << err << std::endl; \
            std::abort();                                      \
        }                                                      \
    } while (0)

namespace kzn
{

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void Engine::init()
{
    // Initialize glfw
    glfwInit();
    // Turn off OpenGl context initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Turn off resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // Initialize window
    _window = glfwCreateWindow(_window_extent.width, _window_extent.height, "Kazan Engine", nullptr, nullptr);

    // Create the core Vulkan instances
    init_vulkan();

    // Create the swapchain
    init_swapchain();

    // Create commands stuff
    init_commands();

    _is_initialized = true;
}


void Engine::cleanup()
{
    if(_is_initialized)
    {
        // Destroy command pool
        vkDestroyCommandPool(_device, _command_pool, nullptr);
        
        // Destroy Swapchain
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);

        // Destroy swapchain resources
        for (int i = 0; i < _swapchain_image_views.size(); i++)
        {
            // Images are implicitly destroyed by the swapchain
            vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
        }

        // Destroy device
        vkDestroyDevice(_device, nullptr);

        // Destroy surface
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        // Destroy Debug messenger
        vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);

        // Destroy instance
        vkDestroyInstance(_instance, nullptr);

        // Destroy window
        glfwDestroyWindow(_window);

        // Destroy glfw
        glfwTerminate();
    }
}


void Engine::draw()
{

}


void Engine::run()
{
    // Main loop
    while(!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();
        draw();
    }
}


void Engine::init_vulkan()
{
    vkb::InstanceBuilder builder;

    // Make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("Hello triangle")
        .request_validation_layers(true)
        .require_api_version(1, 1, 0)
        .use_default_debug_messenger()
        .build();

    vkb::Instance vkb_inst = inst_ret.value();

    // Store the instance
    _instance = vkb_inst.instance;
    // Store the debug messenger
    _debug_messenger = vkb_inst.debug_messenger;



    if(glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    // We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice vkb_physical_device = selector
        .set_minimum_version(1, 1)
        .set_surface(_surface)
        .select()
        .value();

    //create the final Vulkan device
    vkb::DeviceBuilder vkb_device_builder{ vkb_physical_device };

    vkb::Device vkb_device = vkb_device_builder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    _device = vkb_device.device;
    _physical_device = vkb_physical_device.physical_device;

    // Use vkbootstrap to get a Graphics queue & Queue family index
    _graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    _graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();


}


void Engine::init_swapchain()
{
    vkb::SwapchainBuilder vkb_swapchain_builder{ _physical_device, _device, _surface };

    vkb::Swapchain vkb_swapchain = vkb_swapchain_builder
        .use_default_format_selection()
        // Use vsync present mode
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(_window_extent.width, _window_extent.height)
        .build()
        .value();

    // Store swapchain and its related images
    _swapchain = vkb_swapchain.swapchain;
    _swapchain_images = vkb_swapchain.get_images().value();
    _swapchain_image_views = vkb_swapchain.get_image_views().value();

    _swapchain_image_format = vkb_swapchain.image_format;
}


void Engine::init_commands()
{
    // Create a command pool for commands submitted to the graphics queue.
	// We also want the pool to allow for reseting of individual command buffers
    VkCommandPoolCreateInfo command_pool_info = kzn::command_pool_create_info(_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(_device, &command_pool_info, nullptr, &_commandPool));

    // Allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmd_alloc_info = command_buffer_allocate_info(_command_pool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &cmd_alloc_info, &_main_command_buffer));
}

}