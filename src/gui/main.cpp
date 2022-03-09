#include "kazan.hpp"

#include <thread>
#include <functional>
#include <iostream>
#include <string_view>
#include <chrono>

using namespace kzn;

// TODO: New todo list and priority rearrangement


class PerFrameData
{
public:
    PerFrameData(vk::Device* _device, vk::CommandPool& cmd_pool)
        : cmd_buffer(cmd_pool.allocate())
    {
        PerFrameData::device = _device;
        img_available = vk::create_semaphore(*device);
        finished_render = vk::create_semaphore(*device);
        in_flight_fence = vk::create_fence(*device);
        Log::debug("PerFrameData created");
    }

    ~PerFrameData()
    {
        vk::destroy_semaphore(*device, img_available);
        vk::destroy_semaphore(*device, finished_render);
        vk::destroy_fence(*device, in_flight_fence);
        Log::debug("PerFrameData destroyed");
    }

    static std::size_t next() noexcept { return (++PerFrameData::next_idx) % MAX_FRAMES_IN_FLIGHT; }

public:
    static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;

    vk::CommandBuffer cmd_buffer;
    VkSemaphore       img_available;
    VkSemaphore       finished_render;
    VkFence           in_flight_fence;

private:
    inline static std::size_t next_idx = 0;
    inline static vk::Device* device = nullptr;
};

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
                         .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                         .build();

    auto render_pass = vk::RenderPassBuilder(&device)
                           .set_format(swapchain.get_surface_format().format)
                           .build();

    auto pipeline_layout = vk::PipelineLayoutBuilder(&device)
                               .build();
    auto pipeline_config = vk::PipelineConfigBuilder(pipeline_layout, render_pass, swapchain.get_extent())
                               .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) // Optional
                               .set_polygon_mode(VK_POLYGON_MODE_FILL)            // Optional
                               .build();
    auto pipeline = vk::Pipeline(
        &device,
        "assets/shaders/triangle/triangle.vert.spv",
        "assets/shaders/triangle/triangle.frag.spv",
        pipeline_config);

    render_pass.create_framebuffers(swapchain);

    // Create command pool and buffers
    auto cmd_pool = vk::CommandPool(&device);

    // Create all data per frame in flight
    // Initialize command buffers
    // Initialize sync primitives
    std::vector<PerFrameData> per_frame_data;
    per_frame_data.reserve(PerFrameData::MAX_FRAMES_IN_FLIGHT);
    for(std::size_t i = 0; i < PerFrameData::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        per_frame_data.emplace_back(&device, cmd_pool);
    }
    Log::warning("size: {}", per_frame_data.size());

    while (!window.should_close())
    {
        window.poll_events();

        ////////// Draw Section //////////
        // Frame time begin //
        auto begin = std::chrono::high_resolution_clock::now();
        //////////////////////

        static uint32_t fps = 0;
        // Fetch current frame sync structures and cmd buffer
        auto frame_idx = PerFrameData::next();
        auto cmd_buffer = per_frame_data[frame_idx].cmd_buffer;
        auto img_available = per_frame_data[frame_idx].img_available;
        auto finished_render = per_frame_data[frame_idx].finished_render;
        auto in_flight_fence = per_frame_data[frame_idx].in_flight_fence;

        vkWaitForFences(device.vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device.vk_device(), 1, &in_flight_fence);

        swapchain.acquire_next(img_available);
        cmd_buffer.reset();
        cmd_buffer.begin();
        render_pass.begin(cmd_buffer, swapchain);

        pipeline.bind(cmd_buffer);
        vkCmdDraw(cmd_buffer.vk_command_buffer(), 3, 1, 0, 0);

        render_pass.end(cmd_buffer);
        cmd_buffer.end(); // throws ResultError if it fails to record command buffer

        device.graphics_queue_submit(cmd_buffer, img_available, finished_render, in_flight_fence);
        device.present_queue_present(swapchain, finished_render);

        // Frame time begin //
        auto end = std::chrono::high_resolution_clock::now();
        auto seconds = duration_cast<std::chrono::duration<double>>(end - begin).count();
        //////////////////////
        Log::info("FPS: {}", 1 / seconds);
        //////////////////////////////////
    }
    device.wait_idle();

    // List all devices if you want to choose the device
    // auto available_gpus = vk::Device::available_devices(instance);
    // Create default device
    // auto device = vk::Device::default();
}