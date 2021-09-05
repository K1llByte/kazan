#include "engine.hpp"

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

#include "initializers.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VkBootstrap.h"

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdint>


#define VK_CHECK(x)                                            \
    do                                                         \
    {                                                          \
        VkResult err = x;                                      \
        if (err)                                               \
        {                                                      \
            std::cout << "Vulkan Error: " << err << std::endl; \
            std::abort();                                      \
        }                                                      \
    } while(0)

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

    load_meshes();

    _is_initialized = true;
}


void Engine::cleanup()
{
    if(_is_initialized)
    {
        // // Destroy command pool
        // vkDestroyCommandPool(_device, _command_pool, nullptr);
        
        // // Destroy Swapchain
        // vkDestroySwapchainKHR(_device, _swapchain, nullptr);

        // //destroy the main renderpass
        // vkDestroyRenderPass(_device, _render_pass, nullptr);

        // //destroy swapchain resources
        // for(int i = 0; i < _framebuffers.size(); ++i)
        // {
        //     vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
        //     vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
        // }

        // // Destroy swapchain resources
        // for (int i = 0; i < _swapchain_image_views.size(); ++i)
        // {
        //     // Images are implicitly destroyed by the swapchain
        //     vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
        // }

        // Make sure the GPU has stopped doing its things
        // TODO(kb): change timeout to max uint value
        vkWaitForFences(_device, 1, &_render_fence, true, 1000000000);

        _main_deletion_queue.flush();

        // Destroy surface
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        // Destroy device
        vkDestroyDevice(_device, nullptr);

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
    float flash = std::abs(std::sin(_frame_number / 30.f));
    clear_value.color = { { 0.0f, 0.0f, flash, 1.0f } };

    // Clear depth at 1
    VkClearValue depth_clear;
    depth_clear.depthStencil.depth = 1.f;

    // Start the main renderpass.
    // We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    VkRenderPassBeginInfo rp_info{};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.pNext = nullptr;

    rp_info.renderPass = _render_pass;
    rp_info.renderArea.offset.x = 0;
    rp_info.renderArea.offset.y = 0;
    rp_info.renderArea.extent = _window_extent;
    rp_info.framebuffer = _framebuffers[swapchain_image_index];

    // Connect clear values
    rp_info.clearValueCount = 2;
    VkClearValue clear_values[] = { clear_value, depth_clear };
    rp_info.pClearValues = &clear_values[0];

    vkCmdBeginRenderPass(_main_command_buffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    // Once we start adding rendering commands, they will go here

    vkCmdBindPipeline(_main_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _triangle_pipeline);

    // Bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(_main_command_buffer, 0, 1, &_triangle_mesh._vertex_buffer._buffer, &offset);

    // Make a model view matrix for rendering the object
    // Camera position
    glm::vec3 cam_pos = { 0.f,0.f,-3.5f };

    glm::mat4 view = glm::translate(glm::mat4(1.f), cam_pos);
    //camera projection
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;
    //model rotation
    glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(_frame_number * 0.4f), glm::vec3(0, 1, 0));

    //calculate final mesh matrix
    glm::mat4 mesh_matrix = projection * view * model;

    MeshPushConstants constants;
    constants.pvm = mesh_matrix;

    //upload the matrix to the GPU via push constants
    vkCmdPushConstants(_main_command_buffer, _triangle_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

    vkCmdDraw(_main_command_buffer, _triangle_mesh._vertices.size(), 1, 0, 0);

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
    // TODO: cast stuff
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

    
    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = _physical_device;
    allocator_info.device = _device;
    allocator_info.instance = _instance;
    vmaCreateAllocator(&allocator_info, &_allocator);

    _main_deletion_queue.push_function([=]() {
        vmaDestroyAllocator(_allocator);
    });
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

    _main_deletion_queue.push_function([=]() {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    });

    // ========= Depth buffer/image allocation ========= //

    // Depth image size will match the window
    VkExtent3D depth_image_extent = {
        _window_extent.width,
        _window_extent.height,
        1
    };

    // Hardcoding the depth format to 32 bit float
    _depth_format = VK_FORMAT_D32_SFLOAT;

    // The depth image will be an image with the format we selected and Depth Attachment usage flag
    VkImageCreateInfo dimg_info = kzn::image_create_info(_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depth_image_extent);

    // For the depth image, we want to allocate it from GPU local memory
    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // ?

    // Allocate and create the image
    vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depth_image._image, &_depth_image._allocation, nullptr);

    // Build an image-view for the depth image to use for rendering
    VkImageViewCreateInfo dview_info = kzn::imageview_create_info(_depth_format, _depth_image._image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(_device, &dview_info, nullptr, &_depth_image_view));

    // Add to deletion queues
    _main_deletion_queue.push_function([=]() {
        vkDestroyImageView(_device, _depth_image_view, nullptr);
        vmaDestroyImage(_allocator, _depth_image._image, _depth_image._allocation);
    });
}


void Engine::init_commands()
{
    // Create a command pool for commands submitted to the graphics queue.
    // We also want the pool to allow for reseting of individual command buffers
    VkCommandPoolCreateInfo command_pool_info = kzn::command_pool_create_info(_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(_device, &command_pool_info, nullptr, &_command_pool));

    // Allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmd_alloc_info = command_buffer_allocate_info(_command_pool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &cmd_alloc_info, &_main_command_buffer));

    _main_deletion_queue.push_function([=]() {
        vkDestroyCommandPool(_device, _command_pool, nullptr);
    });
}


void Engine::init_default_renderpass()
{
    // ======== Render Target Attachment ======== //

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

    // ======== Depth Buffer Attachment ======== //

    VkAttachmentDescription depth_attachment{};
    // Depth attachment
    depth_attachment.flags = 0;
    depth_attachment.format = _depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // We are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    // hook the depth attachment into the subpass
    subpass.pDepthStencilAttachment = &depth_attachment_ref;


    VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0]; // TODO(kb): change to just attachments
    // Connect the color attachment & depth buffer to the info
    // Connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;


    VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_render_pass));

    _main_deletion_queue.push_function([=]() {
        vkDestroyRenderPass(_device, _render_pass, nullptr);
    });
}


void Engine::init_framebuffers()
{
    // Create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    VkFramebufferCreateInfo fb_info{};
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
    for (int i = 0; i < swapchain_imagecount; ++i)
    {
        VkImageView attachments[2] = { _swapchain_image_views[i], _depth_image_view };

        fb_info.attachmentCount = 2;
        fb_info.pAttachments = attachments;
        VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));
        
        _main_deletion_queue.push_function([=]() {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
            vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
        });
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

    // Enqueue the destruction of the fence
    _main_deletion_queue.push_function([=]() {
        vkDestroyFence(_device, _render_fence, nullptr);
    });

    // For the semaphores we don't need any flags
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_present_semaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_render_semaphore));

    _main_deletion_queue.push_function([=]() {
        vkDestroySemaphore(_device, _present_semaphore, nullptr);
        vkDestroySemaphore(_device, _render_semaphore, nullptr);
    });
}


void Engine::init_pipelines()
{
    VkShaderModule frag_shader;
    if(!load_shader_module("shaders/mesh.frag.spv", &frag_shader))
    {
        std::cout << "Error when building the triangle fragment shader module" << std::endl;
    }
    else
    {
        std::cout << "Triangle fragment shader successfully loaded" << std::endl;
    }

    VkShaderModule vertex_shader;
    if(!load_shader_module("shaders/mesh.vert.spv", &vertex_shader))
    {
        std::cout << "Error when building the triangle vertex shader module" << std::endl;
    }
    else
    {
        std::cout << "Triangle vertex shader successfully loaded" << std::endl;
    }


    // Build the pipeline layout that controls the inputs/outputs of the shader
    // we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
    VkPipelineLayoutCreateInfo pipeline_layout_info = kzn::pipeline_layout_create_info();

    // Setup push constants
    VkPushConstantRange push_constant{};
    // This push constant range starts at the beginning
    push_constant.offset = 0;
    // This push constant range takes up the size of a MeshPushConstants struct
    push_constant.size = sizeof(MeshPushConstants);
    // This push constant range is accessible only in the vertex shader
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    pipeline_layout_info.pPushConstantRanges = &push_constant;
    pipeline_layout_info.pushConstantRangeCount = 1;

    VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_triangle_pipeline_layout));


    // Build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
    PipelineBuilder pipeline_builder;

    pipeline_builder._shader_stages.push_back(
        kzn::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertex_shader));

    pipeline_builder._shader_stages.push_back(
        kzn::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, frag_shader));

    VertexInputDescription vertex_description = Vertex::get_vertex_description();

    // Vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    pipeline_builder._vertex_input_info = kzn::vertex_input_state_create_info();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipeline_builder._vertex_input_info.pVertexAttributeDescriptions = vertex_description.attributes.data();
    pipeline_builder._vertex_input_info.vertexAttributeDescriptionCount = vertex_description.attributes.size();

    pipeline_builder._vertex_input_info.pVertexBindingDescriptions = vertex_description.bindings.data();
    pipeline_builder._vertex_input_info.vertexBindingDescriptionCount = vertex_description.bindings.size();

    // Input assembly is the configuration for drawing triangle lists, strips, or individual points.
    // we are just going to draw triangle list
    pipeline_builder._input_assembly = kzn::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    // Build viewport and scissor from the swapchain extents
    pipeline_builder._viewport.x = 0.0f;
    pipeline_builder._viewport.y = 0.0f;
    pipeline_builder._viewport.width = (float) _window_extent.width;
    pipeline_builder._viewport.height = (float) _window_extent.height;
    pipeline_builder._viewport.minDepth = 0.0f;
    pipeline_builder._viewport.maxDepth = 1.0f;

    pipeline_builder._scissor.offset = { 0, 0 };
    pipeline_builder._scissor.extent = _window_extent;

    // Configure the rasterizer to draw filled triangles
    pipeline_builder._rasterizer = kzn::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    // We don't use multisampling, so just run the default one
    pipeline_builder._multisampling = kzn::multisampling_state_create_info();

    // A single blend attachment with no blending and writing to RGBA
    pipeline_builder._color_blend_attachment = kzn::color_blend_attachment_state();

    // Use the triangle layout we created
    pipeline_builder._pipeline_layout = _triangle_pipeline_layout;

    // Default depthtesting
    pipeline_builder._depth_stencil = kzn::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    // Finally build the pipeline
    _triangle_pipeline = pipeline_builder.build(_device, _render_pass);


    vkDestroyShaderModule(_device, vertex_shader, nullptr);
    vkDestroyShaderModule(_device, frag_shader, nullptr);

    _main_deletion_queue.push_function([=]() {
        // Destroy the pipelines we have created
        vkDestroyPipeline(_device, _triangle_pipeline, nullptr);

        // Destroy the pipeline layout that they use
        vkDestroyPipelineLayout(_device, _triangle_pipeline_layout, nullptr);
    });
}


VkPipeline PipelineBuilder::build(VkDevice device, VkRenderPass pass)
{
    // Make viewport state from our stored viewport and scissor.
    // at the moment we won't support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;

    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &_viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &_scissor;

    // Setup dummy color blending. We aren't using transparent objects yet
    // the blending is just "no blend", but we do write to the color attachment
    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.pNext = nullptr;

    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &_color_blend_attachment;


    // Build the actual pipeline
    // we now use all of the info structs we have been writing into into this one to create the pipeline
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = nullptr;
    pipeline_info.pDepthStencilState = &_depth_stencil;

    pipeline_info.stageCount = _shader_stages.size();
    pipeline_info.pStages = _shader_stages.data();
    pipeline_info.pVertexInputState = &_vertex_input_info;
    pipeline_info.pInputAssemblyState = &_input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &_rasterizer;
    pipeline_info.pMultisampleState = &_multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.layout = _pipeline_layout;
    pipeline_info.renderPass = pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    // It's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline new_pipeline;
    if(vkCreateGraphicsPipelines(
        device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &new_pipeline) != VK_SUCCESS)
    {
        std::cout << "failed to create pipeline\n";
        return VK_NULL_HANDLE; // failed to create graphics pipeline
    }
    else
    {
        return new_pipeline;
    }
}


void Engine::load_meshes()
{
    // Make the array 3 vertices long
    // _triangle_mesh._vertices.resize(6);

    // // Vertex positions
    // _triangle_mesh._vertices[0].position = {  0.5f,  0.5f, 0.0f };
    // _triangle_mesh._vertices[1].position = { -0.5f,  0.5f, 0.0f };
    // _triangle_mesh._vertices[2].position = { -0.5f, -0.5f, 0.0f };
    // _triangle_mesh._vertices[3].position = { -0.5f, -0.5f, 0.0f };
    // _triangle_mesh._vertices[4].position = {  0.5f, -0.5f, 0.0f };
    // _triangle_mesh._vertices[5].position = {  0.5f,  0.5f, 0.0f };

    // // Vertex colors, all green
    // _triangle_mesh._vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
    // _triangle_mesh._vertices[1].color = { 1.f, 1.f, 0.0f }; //pure green
    // _triangle_mesh._vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green
    // _triangle_mesh._vertices[3].color = { 0.f, 1.f, 0.0f }; //pure green
    // _triangle_mesh._vertices[4].color = { 1.f, 1.f, 0.0f }; //pure green
    // _triangle_mesh._vertices[5].color = { 0.f, 1.f, 0.0f }; //pure green

    // We don't care about the vertex normals

    _triangle_mesh.load_from_obj("models/monkey.obj");
    upload_mesh(_triangle_mesh);
}


void Engine::upload_mesh(Mesh& mesh)
{
    // Allocate vertex buffer
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // This is the total size, in bytes, of the buffer we are allocating
    buffer_info.size = mesh._vertices.size() * sizeof(Vertex);
    // This buffer is going to be used as a Vertex Buffer
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


    // Let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vma_alloc_info{};
    vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    // Allocate the buffer
    VK_CHECK(vmaCreateBuffer(
        _allocator,
        &buffer_info,
        &vma_alloc_info,
        &mesh._vertex_buffer._buffer,
        &mesh._vertex_buffer._allocation,
        nullptr));

    // Add the destruction of triangle mesh buffer to the deletion queue
    _main_deletion_queue.push_function([=]() {
        vmaDestroyBuffer(_allocator, mesh._vertex_buffer._buffer, mesh._vertex_buffer._allocation);
    });


    // Copy vertex data
    void* data;
    vmaMapMemory(_allocator, mesh._vertex_buffer._allocation, &data);

    std::memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(_allocator, mesh._vertex_buffer._allocation);
}

}