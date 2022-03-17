#include "core/renderer.hpp"

#include "vk/device_features.hpp"
#include "vk/utils.hpp"

namespace kzn
{
    Renderer::Renderer(Window* window)
        : window(window),
        // Create Vulkan Instance
        instance(vk::InstanceBuilder()
            .enable_validation_layers()
            .set_extensions(window->required_extensions())
            .build()),
        // Get Surface
        surface(instance.create_surface(window->glfw_ptr())),
        // Create device
        device(vk::DeviceBuilder(instance)
            .set_surface(surface)
            // NOTE: IF THIS EXTENSION ISN'T LOADED THEN THE
            // SwapchainBuilder will seg fault
            .set_extensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
            .set_features(vk::as_vk_device_features({
                vk::DeviceFeature::SamplerAnisotropy
            }))
            .build()),
        // Create Swapchain
        swapchain(vk::SwapchainBuilder(&device, surface, window->extent())
            .set_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .build()),
        // Create command pool and buffers
        cmd_pool(&device)
    {
        // Create all data per frame in flight
        // Initialize command buffers
        // Initialize sync primitives
        // 2x In Flight Frame Data
        // 3x Image Fences
        image_fences.resize(swapchain.num_images());
        per_frame_data.reserve(PerFrameData::MAX_FRAMES_IN_FLIGHT);
        for(std::size_t i = 0; i < PerFrameData::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            per_frame_data.emplace_back(&device, cmd_pool);
        }

        const auto window_extent = window->extent();
        viewport = vk::create_viewport(window_extent);
        scissor = vk::create_scissor(window_extent);
    }
    
    Renderer::~Renderer()
    {

    }

    void Renderer::add_render_pass(vk::RenderPass& render_pass)
    {
        render_passes.push_back(&render_pass);
    }

    void Renderer::render_frame(std::function<void(void)> draw_func)
    {
        /////////////////
        // Begin Frame //
        /////////////////

        // Fetch current frame sync structures and cmd buffer
        auto frame_idx = PerFrameData::next();
        auto cmd_buffer = per_frame_data[frame_idx].cmd_buffer;
        current_command_buffer = &per_frame_data[frame_idx].cmd_buffer;
        auto img_available = per_frame_data[frame_idx].img_available;
        auto finished_render = per_frame_data[frame_idx].finished_render;
        auto in_flight_fence = per_frame_data[frame_idx].in_flight_fence;
        
        vkWaitForFences(device.vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        uint32_t image_idx;
        try {
            image_idx = swapchain.acquire_next(img_available);
        }
        catch(const vk::SwapchainResized&) {
            // Swapchain recreation
            auto win_extent = window->extent();
            swapchain.recreate(win_extent);
            viewport = vk::create_viewport(win_extent);
            scissor = vk::create_scissor(win_extent);
            // TODO: Research more about this Framebuffer recreation
            for(auto render_pass : render_passes)
            {
                render_pass->recreate_framebuffers(swapchain);
            }
            return;
        }

        cmd_buffer.reset();
        cmd_buffer.begin();

        /////////////////
        //    Draw     //
        /////////////////

        draw_func();

        // RenderSystem commands
        // auto& cmd_buffer = renderer.current_cmd_buffer();
        // auto& viewport = renderer.current_viewport();
        // auto& scissor = renderer.current_scissor();

        // render_pass.begin(cmd_buffer, renderer.swapchain());
        // pipeline.set_viewport(cmd_buffer, viewport);
        // pipeline.set_scissor(cmd_buffer, scissor);
        // pipeline.bind(cmd_buffer);

        // Model draw commands
        // vbo.bind(cmd_buffer);
        // ibo.bind(cmd_buffer);
        // vkCmdDrawIndexed(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0, 0);

        /////////////////
        //  End Frame  //
        /////////////////

        // Stop registering commands to the cmd buffer
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
        catch(const vk::SwapchainResized&) {
            window->set_resized(true);
        }
        if(window->was_resized())
        {
            // Swapchain recreation
            auto win_extent = window->extent();
            swapchain.recreate(win_extent);
            viewport = vk::create_viewport(win_extent);
            scissor = vk::create_scissor(win_extent);
            // TODO: Research more about this Framebuffer recreation
            for(auto render_pass : render_passes)
            {
                render_pass->recreate_framebuffers(swapchain);
            }
        }

    }
} // namespace kzn