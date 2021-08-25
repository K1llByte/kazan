#include "engine.hpp"

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

// #include "types.hpp"
#include "initializers.hpp"

#include "VkBootstrap.h"

#include <cmath>

#include <iostream>
#include <fstream>

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

    // Create Renderpass
    init_default_renderpass();

    // Create framebuffers
    init_framebuffers();

    // Create sync structures
    init_sync_structures();

    // Create pipeline
    init_pipelines();

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

        //destroy the main renderpass
        vkDestroyRenderPass(_device, _render_pass, nullptr);

        //destroy swapchain resources
        for(int i = 0; i < _framebuffers.size(); ++i)
        {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
            vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
        }

        // Destroy swapchain resources
        for (int i = 0; i < _swapchain_image_views.size(); ++i)
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
    // Wait until the GPU has finished rendering the last frame. Timeout of 1 second
    VK_CHECK(vkWaitForFences(_device, 1, &_render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(_device, 1, &_render_fence));

    // Request image from the swapchain, one second timeout
    uint32_t swapchain_image_index;
    VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _present_semaphore, nullptr, &swapchain_image_index));

    // Now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(_main_command_buffer, 0));

    // Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
    VkCommandBufferBeginInfo cmd_begin_info{};
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_begin_info.pNext = nullptr;

    cmd_begin_info.pInheritanceInfo = nullptr;
    cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(_main_command_buffer, &cmd_begin_info));


    // Make a clear-color from frame number. This will flash with a 120*pi frame period.
    VkClearValue clear_value;
    float flash = abs(sin(_frame_number / 120.f));
    clear_value.color = { { 0.0f, 0.0f, flash, 1.0f } };

    //start the main renderpass.
    //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    VkRenderPassBeginInfo rp_info{};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.pNext = nullptr;

    rp_info.renderPass = _render_pass;
    rp_info.renderArea.offset.x = 0;
    rp_info.renderArea.offset.y = 0;
    rp_info.renderArea.extent = _window_extent;
    rp_info.framebuffer = _framebuffers[swapchain_image_index];

    //connect clear values
    rp_info.clearValueCount = 1;
    rp_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(_main_command_buffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    // Finalize the render pass
    vkCmdEndRenderPass(_main_command_buffer);
    // Finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(_main_command_buffer));


    // Prepare the submission to the queue.
    // We want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
    // We will signal the _renderSemaphore, to signal that rendering has finished

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &wait_stage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &_present_semaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &_render_semaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &_main_command_buffer;

    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit(_graphics_queue, 1, &submit, _render_fence));

    // This will put the image we just rendered into the visible window.
    // We want to wait on the _renderSemaphore for that,
    // as it's necessary that drawing commands have finished before the image is displayed to the user
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;

    present_info.pSwapchains = &_swapchain;
    present_info.swapchainCount = 1;

    present_info.pWaitSemaphores = &_render_semaphore;
    present_info.waitSemaphoreCount = 1;

    present_info.pImageIndices = &swapchain_image_index;

    VK_CHECK(vkQueuePresentKHR(_graphics_queue, &present_info));

    // increase the number of frames drawn
    ++_frame_number;
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


bool Engine::load_shader_module(const char* file_path, VkShaderModule* out_shader_module)
{
    // Open the file. With cursor at the end
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return false;
    
    //find what the size of the file is by looking up the location of the cursor
    //because the cursor is at the end, it gives the size directly in bytes
    size_t file_size = (size_t) file.tellg();
    
    // SPIRV expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
    
    // Put file cursor at beginning
    file.seekg(0);
    
    // Load the entire file into the buffer
    file.read((char*)buffer.data(), file_size);
    
    file.close();

    //create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;

    // code_size has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
    create_info.codeSize = buffer.size() * sizeof(uint32_t);
    create_info.pCode = buffer.data();

    // Check that the creation goes well.
    VkShaderModule shader_module;
    if(vkCreateShaderModule(_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        return false;
    }

    *out_shader_module = shader_module;

    return true;
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


void Engine::init_default_renderpass()
{
    // The renderpass will use this color attachment.
    VkAttachmentDescription color_attachment{};
    // The attachment will have the format needed by the swapchain
    color_attachment.format = _swapchain_image_format;
    // 1 sample, we won't be doing MSAA
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // We Clear when this attachment is loaded
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // We keep the attachment stored when the renderpass ends
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // We don't care about stencil
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // We don't know or care about the starting layout of the attachment
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // After the renderpass ends, the image has to be on a layout ready for display
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    // Attachment number will index into the pAttachments array in the parent renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;


    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // Connect the color attachment to the info
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    // Connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;


    VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_render_pass));
}


void Engine::init_framebuffers()
{
    // Create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = _render_pass;
    fb_info.attachmentCount = 1;
    fb_info.width = _window_extent.width;
    fb_info.height = _window_extent.height;
    fb_info.layers = 1;

    // Grab how many images we have in the swapchain
    const uint32_t swapchain_imagecount = _swapchain_images.size();
    // TODO: change copy to resize
    // _framebuffers.resize(swapchain_imagecount);
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    // Create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++)
    {
        fb_info.pAttachments = &_swapchain_image_views[i];
        VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));
    }
}


void Engine::init_sync_structures()
{
    // Create synchronization structures
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;

    // We want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_device, &fence_create_info, nullptr, &_render_fence));

    // For the semaphores we don't need any flags
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_present_semaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_render_semaphore));
}


void Engine::init_pipelines()
{
    VkShaderModule frag_shader;
    if(!load_shader_module("../../shaders/triangle.frag.spv", &frag_shader))
    {
        std::cout << "Error when building the triangle fragment shader module" << std::endl;
    }
    else
    {
        std::cout << "Triangle fragment shader successfully loaded" << std::endl;
    }

    VkShaderModule vertex_shader;
    if(!load_shader_module("../../shaders/triangle.vert.spv", &vertex_shader))
    {
        std::cout << "Error when building the triangle vertex shader module" << std::endl;
    }
    else
    {
        std::cout << "Triangle vertex shader successfully loaded" << std::endl;
    }
}

}