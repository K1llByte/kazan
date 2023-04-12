#include "core/renderer.hpp"

#include "core/context.hpp"
#include "core/events/event_handlers.hpp"
#include "vk/device_features.hpp"
#include "vk/utils.hpp"


namespace kzn
{
    Renderer::Renderer(Window* window)
        : m_window(window),
        // Initialize Context
        m_context(Context::create(*m_window)),
        // Create command pool and buffers
        m_cmd_pool(&Context::device())
    {
        // Create all data per frame in flight
        // Initialize command buffers
        // Initialize sync primitives
        // 2x In Flight Frame Data
        // 3x Image Fences
        m_image_fences.resize(Context::swapchain().num_images());
        m_per_frame_data.reserve(PerFrameData::MAX_FRAMES_IN_FLIGHT);
        for(std::size_t i = 0; i < PerFrameData::MAX_FRAMES_IN_FLIGHT; ++i) {
            m_per_frame_data.emplace_back(&Context::device(), m_cmd_pool);
        }

        const auto window_extent = m_window->extent();
        m_viewport = vk::create_viewport(window_extent);
        m_scissor = vk::create_scissor(window_extent);
    }
    
    Renderer::~Renderer()
    {

    }

    void Renderer::render_frame(std::function<void(vk::CommandBuffer&)> draw_func)
    {
        /////////////////
        // Begin Frame //
        /////////////////

        // Fetch current frame sync structures and cmd buffer
        auto frame_idx = PerFrameData::next();
        auto cmd_buffer = m_per_frame_data[frame_idx].cmd_buffer;
        auto img_available = m_per_frame_data[frame_idx].img_available;
        auto finished_render = m_per_frame_data[frame_idx].finished_render;
        auto in_flight_fence = m_per_frame_data[frame_idx].in_flight_fence;
        
        vkWaitForFences(Context::device().vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        uint32_t image_idx;
        try {
            image_idx = Context::swapchain().acquire_next(img_available);
        }
        catch(const vk::SwapchainResized&) {
            // Swapchain recreation
            auto win_extent = m_window->extent();
            Context::swapchain().recreate(win_extent);
            m_viewport = vk::create_viewport(win_extent);
            m_scissor = vk::create_scissor(win_extent);
            // Resize event submition
            EventManager::submit(ResizeEvent{});
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

        if(m_image_fences[image_idx] != VK_NULL_HANDLE) {
            vkWaitForFences(Context::device().vk_device(), 1, &m_image_fences[image_idx], VK_TRUE, UINT64_MAX);
        }

        m_image_fences[image_idx] = in_flight_fence;
        vkResetFences(Context::device().vk_device(), 1, &in_flight_fence);
        Context::device().graphics_queue_submit(cmd_buffer, img_available, finished_render, in_flight_fence);
        try {
            Context::device().present_queue_present(Context::swapchain(), finished_render);
        }
        catch(const vk::SwapchainResized&) {
            m_window->set_resized(true);
        }
        if(m_window->was_resized())
        {
            // Swapchain recreation
            auto win_extent = m_window->extent();
            Context::swapchain().recreate(win_extent);
            m_viewport = vk::create_viewport(win_extent);
            m_scissor = vk::create_scissor(win_extent);
            // Resize callback
            EventManager::submit(ResizeEvent{});
        }

        Context::device().wait_idle();
    }
} // namespace kzn