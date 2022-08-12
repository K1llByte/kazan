#include "core/renderer.hpp"

#include "core/context.hpp"
#include "vk/device_features.hpp"
#include "vk/utils.hpp"


namespace kzn
{
    Renderer::Renderer(Window* window)
        : window(window),
        // Initialize Context
        context(Context::create(window)),
        // Create Vulkan Instance
        // Create device
        // Create Swapchain
        // Create command pool and buffers
        cmd_pool(&Context::device())
    {
        // Create all data per frame in flight
        // Initialize command buffers
        // Initialize sync primitives
        // 2x In Flight Frame Data
        // 3x Image Fences
        image_fences.resize(Context::swapchain().num_images());
        per_frame_data.reserve(PerFrameData::MAX_FRAMES_IN_FLIGHT);
        for(std::size_t i = 0; i < PerFrameData::MAX_FRAMES_IN_FLIGHT; ++i) {
            per_frame_data.emplace_back(&Context::device(), cmd_pool);
        }

        const auto window_extent = window->extent();
        viewport = vk::create_viewport(window_extent);
        scissor = vk::create_scissor(window_extent);
    }
    
    Renderer::~Renderer()
    {
        Log::debug("Framebuffers destroyed");
        // Context::destroy();
    }

    void Renderer::add_render_pass(vk::RenderPass& render_pass)
    {
        // Add render pass
        render_passes.push_back(&render_pass);
    }

    void Renderer::render_frame(std::function<void(vk::CommandBuffer&)> draw_func)
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
        
        vkWaitForFences(Context::device().vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        uint32_t image_idx;
        try {
            image_idx = Context::swapchain().acquire_next(img_available);
        }
        catch(const vk::SwapchainResized&) {
            // Swapchain recreation
            auto win_extent = window->extent();
            Context::swapchain().recreate(win_extent);
            viewport = vk::create_viewport(win_extent);
            scissor = vk::create_scissor(win_extent);
            // Resize callback
            resize_callback();
            return;
        }

        cmd_buffer.reset();
        cmd_buffer.begin();

        /////////////////
        //    Draw     //
        /////////////////

        draw_func(cmd_buffer);

        /////////////////
        //  End Frame  //
        /////////////////

        // Stop registering commands to the cmd buffer
        cmd_buffer.end(); // throws ResultError if it fails to record command buffer

        if(image_fences[image_idx] != VK_NULL_HANDLE) {
            vkWaitForFences(Context::device().vk_device(), 1, &image_fences[image_idx], VK_TRUE, UINT64_MAX);
        }
        image_fences[image_idx] = in_flight_fence;
        vkResetFences(Context::device().vk_device(), 1, &in_flight_fence);
        Context::device().graphics_queue_submit(cmd_buffer, img_available, finished_render, in_flight_fence);
        try {
            Context::device().present_queue_present(Context::swapchain(), finished_render);
        }
        catch(const vk::SwapchainResized&) {
            window->set_resized(true);
        }
        if(window->was_resized())
        {
            // Swapchain recreation
            auto win_extent = window->extent();
            Context::swapchain().recreate(win_extent);
            viewport = vk::create_viewport(win_extent);
            scissor = vk::create_scissor(win_extent);
            // Resize callback
            resize_callback();
        }

        Context::device().wait_idle();
    }
} // namespace kzn