#include "kazan.hpp"

#include <thread>
#include <functional>
#include <iostream>
#include <string_view>

using namespace kzn;

// TODO: New todo list and priority rearrangement

int main()
{
    auto window = Window("Hello World!", 800, 600);
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .set_extensions(window.required_extensions())
        .build();
    auto surface = instance.create_surface(window.ptr());

    auto device = vk::DeviceBuilder(instance)
        .set_surface(surface)
        // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE SwapchainBuilder
        // will give a seg fault
        .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
        .build();
    
    auto swapchain = vk::SwapchainBuilder(&device, surface, window.extent())
        .build();

    auto render_pass = vk::RenderPassBuilder(&device)
        .set_format(swapchain.get_surface_format().format)
        .build();

    auto pipeline_layout = vk::PipelineLayoutBuilder(&device)
        .build();
    auto pipeline_config = vk::PipelineConfigBuilder(pipeline_layout, render_pass, swapchain.get_extent())
        .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) // Optional
        .set_polygon_mode(VK_POLYGON_MODE_FILL) // Optional
        .build();
    auto pipeline = vk::Pipeline(
        &device,
        "assets/shaders/triangle/triangle.vert.spv",
        "assets/shaders/triangle/triangle.frag.spv",
        pipeline_config
    );

    render_pass.create_framebuffers(swapchain);

    // Create command pool and buffers
    auto cmd_pool = vk::CommandPool(&device);
    auto cmd_buffer = cmd_pool.allocate();

    // Initialize sync primitives
    VkSemaphore img_available = vk::create_semaphore(device);
    VkSemaphore finished_render = vk::create_semaphore(device);
    VkFence in_flight_fence = vk::create_fence(device);

    while(!window.should_close())
    {
        window.poll_events();

        ////////// Draw Section //////////
        vkWaitForFences(device.vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device.vk_device(), 1, &in_flight_fence);

        auto img_idx = swapchain.acquire_next(img_available);

        cmd_buffer.reset();
        cmd_buffer.begin();
        render_pass.begin(cmd_buffer, swapchain);
        pipeline.bind(cmd_buffer);
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);
        // Log::info("Drawed");
        render_pass.end(cmd_buffer);
        cmd_buffer.end(); // throws ResultError if it fails to record command buffer

        device.graphics_queue_submit(cmd_buffer, img_available, finished_render, in_flight_fence);
        device.present_queue_present(swapchain, finished_render);
        //////////////////////////////////
    }
    device.wait_idle();

    vk::destroy_semaphore(device, img_available);
    vk::destroy_semaphore(device, finished_render);
    vk::destroy_fence(device, in_flight_fence);

    // List all devices if you want to choose the device
    // auto available_gpus = vk::Device::available_devices(instance);
    // Create default device
    // auto device = vk::Device::default();
}