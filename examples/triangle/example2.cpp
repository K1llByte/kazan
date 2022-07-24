#include "kazan.hpp"

#include <thread>
#include <functional>
#include <iostream>
#include <string_view>
#include <chrono>


//////////////////////// Example 2 ////////////////////////
// This example uses the kazan vulkan wrapper to render a
// triangle using vertex input data and rendering using
// vertices and color feeded from the application.
///////////////////////////////////////////////////////////

namespace ex
{
    class PerFrameData
    {
    public:
        PerFrameData(kzn::vk::Device* _device, kzn::vk::CommandPool& cmd_pool)
            : cmd_buffer(cmd_pool.allocate())
        {
            PerFrameData::device = _device;
            img_available = kzn::vk::create_semaphore(*device);
            finished_render = kzn::vk::create_semaphore(*device);
            in_flight_fence = kzn::vk::create_fence(*device);
            kzn::Log::debug("PerFrameData created");
        }

        ~PerFrameData()
        {
            kzn::vk::destroy_semaphore(*device, img_available);
            kzn::vk::destroy_semaphore(*device, finished_render);
            kzn::vk::destroy_fence(*device, in_flight_fence);
            kzn::Log::debug("PerFrameData destroyed");
        }

        // static std::size_t current() noexcept { return PerFrameData::next_idx; }
        static std::size_t next() noexcept { return (++PerFrameData::next_idx) % MAX_FRAMES_IN_FLIGHT; }

    public:
        static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;

        kzn::vk::CommandBuffer cmd_buffer;
        VkSemaphore       img_available;
        VkSemaphore       finished_render;
        VkFence           in_flight_fence;

    private:
        inline static std::size_t next_idx = 0;
        inline static kzn::vk::Device* device = nullptr;
    };
};

struct Vertex
{
    glm::vec2 position;
    glm::vec3 color;
};

int main() try
{
    auto window = kzn::Window("Kazan", 1720, 890);
    auto instance = kzn::vk::InstanceBuilder()
                        .enable_validation_layers()
                        .set_extensions(window.required_extensions())
                        .build();
    auto surface = instance.create_surface(window);

    auto device = kzn::vk::DeviceBuilder(instance)
                      .set_surface(surface)
                      // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE SwapchainBuilder
                      // will give a seg fault
                      .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
                      .set_features(kzn::vk::as_vk_device_features({
                          kzn::vk::DeviceFeature::SamplerAnisotropy
                      }))
                      .build();

    auto swapchain = kzn::vk::SwapchainBuilder(&device, surface, window.extent())
                         .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                         .build();

    auto render_pass = kzn::vk::RenderPassBuilder(&device)
                           .set_format(swapchain.get_surface_format().format)
                           .build();

    auto pipeline_layout = kzn::vk::PipelineLayoutBuilder(&device)
                               .build();
    auto pipeline_config = kzn::vk::PipelineConfigBuilder(pipeline_layout, render_pass)
                               .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR}) // Optional
                               .set_front_face(VK_FRONT_FACE_CLOCKWISE)
                               .set_vtx_input<glm::vec2, glm::vec3>()
                               .build();
    auto pipeline = kzn::vk::Pipeline(
        &device,
        "assets/shaders/triangle_input/triangle.vert.spv",
        "assets/shaders/triangle_input/triangle.frag.spv",
        pipeline_config);

    render_pass.create_framebuffers(swapchain);

    // Create command pool and buffers
    auto cmd_pool = kzn::vk::CommandPool(&device);

    // Create all data per frame in flight
    // Initialize command buffers
    // Initialize sync primitives
    // 2x In Flight Frame Data
    std::vector<ex::PerFrameData> per_frame_data;
    // 3x Image Fences
    std::vector<VkFence> image_fences(swapchain.num_images());
    per_frame_data.reserve(ex::PerFrameData::MAX_FRAMES_IN_FLIGHT);
    for(std::size_t i = 0; i < ex::PerFrameData::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        per_frame_data.emplace_back(&device, cmd_pool);
    }

    std::vector<Vertex> vertices{
        {{ 0.0, -0.5}, {0.984, 0.286, 0.203}},
        {{ 0.5,  0.5}, {0.556, 0.752, 0.486}},
        {{-0.5,  0.5}, {0.513, 0.647, 0.596}}
    };
    std::vector<uint32_t> indices{0, 1, 2};
    
    auto vbo = kzn::vk::VertexBuffer(&device, vertices.size() * sizeof(Vertex));
    vbo.upload(reinterpret_cast<float*>(vertices.data()));
    auto ibo = kzn::vk::IndexBuffer(&device, vertices.size() * sizeof(uint32_t));
    ibo.upload(indices.data());

    const auto window_extent = window.extent();
    auto viewport = kzn::vk::create_viewport(window_extent);
    auto scissor = kzn::vk::create_scissor(window_extent);
    while (!window.is_closed())
    {
        window.poll_events();

        ////////// Draw Section //////////
        // Frame time begin //
        auto begin = std::chrono::high_resolution_clock::now();
        //////////////////////

        // Fetch current frame sync structures and cmd buffer
        auto frame_idx = ex::PerFrameData::next();
        auto cmd_buffer = per_frame_data[frame_idx].cmd_buffer;
        auto img_available = per_frame_data[frame_idx].img_available;
        auto finished_render = per_frame_data[frame_idx].finished_render;
        auto in_flight_fence = per_frame_data[frame_idx].in_flight_fence;

        vkWaitForFences(device.vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        uint32_t image_idx;
        try {
            image_idx = swapchain.acquire_next(img_available);
        }
        catch(const kzn::vk::SwapchainResized&) {
            // Swapchain recreation
            auto win_extent = window.extent();
            swapchain.recreate(win_extent);
            viewport = kzn::vk::create_viewport(win_extent);
            scissor = kzn::vk::create_scissor(win_extent);
            render_pass.recreate_framebuffers(swapchain);
            // image_fences.resize(swapchain.num_images(), VK_NULL_HANDLE);
            continue;
        }

        cmd_buffer.reset();
        cmd_buffer.begin();
        render_pass.begin(cmd_buffer, swapchain);

        pipeline.set_viewport(cmd_buffer, viewport);
        pipeline.set_scissor(cmd_buffer, scissor);
        pipeline.bind(cmd_buffer);
        // kzn::Log::info("Drawed");
        vbo.bind(cmd_buffer);
        ibo.bind(cmd_buffer);
        vkCmdDrawIndexed(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0, 0);
        render_pass.end(cmd_buffer);
        cmd_buffer.end(); // throws ResultError if it fails to record command buffer

        if(image_fences[image_idx] != VK_NULL_HANDLE)
        {
            vkWaitForFences(device.vk_device(), 1, &image_fences[image_idx], VK_TRUE, UINT64_MAX);
        }
        image_fences[image_idx] = in_flight_fence;
        vkResetFences(device.vk_device(), 1, &in_flight_fence);
        device.graphics_queue_submit(cmd_buffer, img_available, finished_render, in_flight_fence);
        try {
            device.present_queue_present(swapchain, finished_render);
        }
        catch(const kzn::vk::SwapchainResized&) {
            window.set_resized(true);
        }
        if(window.was_resized())
        {
            // Swapchain recreation
            auto win_extent = window.extent();
            swapchain.recreate(win_extent);
            viewport = kzn::vk::create_viewport(win_extent);
            scissor = kzn::vk::create_scissor(win_extent);
            render_pass.recreate_framebuffers(swapchain);
            // image_fences.resize(swapchain.num_images(), VK_NULL_HANDLE);
        }

        // Frame time end //
        auto end = std::chrono::high_resolution_clock::now();
        auto seconds = duration_cast<std::chrono::duration<double>>(end - begin).count();
        //////////////////////
        window.set_title(fmt::format("FPS: {:.0f}", 1 / seconds));
        //////////////////////////////////
    }
    device.wait_idle();

    // List all devices if you want to choose the device
    // auto available_gpus = kzn::vk::Device::available_devices(instance);
    // Create default device
    // auto device = kzn::vk::Device::default();
}
catch(const kzn::vk::ResultError& re)
{
    kzn::Log::error("ResultError: {}", re.raw());
}