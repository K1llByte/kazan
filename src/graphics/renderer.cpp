#include "renderer.hpp"

#include "events/event.hpp"
#include "events/event_manager.hpp"
#include "vk/utils.hpp"
#include <stdexcept>

namespace kzn {

PerFrameData::PerFrameData(vk::Device& device, vk::CommandPool& cmd_pool)
    : m_device{device}
    , cmd_buffer(cmd_pool.allocate()) {
    img_available = vk::create_semaphore(m_device);
    finished_render = vk::create_semaphore(m_device);
    in_flight_fence = vk::create_fence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
}

PerFrameData::PerFrameData(PerFrameData&& other)
    : cmd_buffer{std::move(other.cmd_buffer)}
    , img_available{other.img_available}
    , finished_render{other.finished_render}
    , in_flight_fence{other.in_flight_fence}
    , m_device{other.m_device} {
    other.in_flight_fence = VK_NULL_HANDLE;
}

PerFrameData::~PerFrameData() {
    if (in_flight_fence != VK_NULL_HANDLE) {
        vk::destroy_semaphore(m_device, img_available);
        vk::destroy_semaphore(m_device, finished_render);
        vk::destroy_fence(m_device, in_flight_fence);
    }
}

////////////////////////////////////////////////////////////////

Renderer::Renderer(Window& window)
    : m_window{window}
    , m_instance({
          .extensions = window.required_extensions(),
          .with_validation = true,
      })
    , m_surface(window.create_surface(m_instance))
    , m_device(
          m_instance,
          {.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, .surface = m_surface
          }
      )
    , m_swapchain(m_device, m_surface, window.extent())
    , m_dset_allocator(m_device)
    , m_dset_layout_cache(m_device)
    , m_cmd_pool(m_device) {
    // Per frame data
    m_frame_data.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_frame_data.emplace_back(device(), m_cmd_pool);
    }
}

void Renderer::render_frame(const RenderFrameFunc& render_func) {

    /////////////////
    // Begin Frame //
    /////////////////

    auto& cmd_buffer = m_frame_data[m_frame_idx].cmd_buffer;
    auto& img_available = m_frame_data[m_frame_idx].img_available;
    auto& finished_render = m_frame_data[m_frame_idx].finished_render;
    auto& in_flight_fence = m_frame_data[m_frame_idx].in_flight_fence;

    // Wait for previous frame
    vkWaitForFences(device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);

    // Acquire next frame
    auto opt_image_index = swapchain().acquire_next(img_available);
    if (!opt_image_index.has_value()) {
        swapchain().recreate(m_window.extent());
        EventManager::send(SwapchainResize{});
        return;
    }
    uint32_t image_index = opt_image_index.value();

    // Only reset the fence if we are submitting work
    vkResetFences(device(), 1, &in_flight_fence);

    cmd_buffer.reset();
    cmd_buffer.begin();

    /////////////////
    //    Draw     //
    /////////////////

    render_func(cmd_buffer);

    /////////////////
    //  End Frame  //
    /////////////////

    // Stop registering commands to the cmd buffer
    cmd_buffer.end();

    // Submit command buffer
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &img_available;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    VkCommandBuffer cmd_buffers[] = {cmd_buffer.vk_cmd_buffer()};
    submit_info.pCommandBuffers = cmd_buffers;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &finished_render;

    auto result = vkQueueSubmit(
        device().graphics_queue().vk_queue, 1, &submit_info, in_flight_fence
    );
    VK_CHECK_MSG(result, "Failed to submit command buffer!");

    // Preset frame
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &finished_render;
    VkSwapchainKHR swapchains[] = {swapchain().vk_swapchain()};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result =
        vkQueuePresentKHR(device().present_queue().vk_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_window.was_resized()) {
        swapchain().recreate(m_window.extent());
        EventManager::send(SwapchainResize{});
    }
    else if (result != VK_SUCCESS) {
        throw vk::ResultError(result);
    }

    // Increment frame in flight index
    m_frame_idx = (m_frame_idx + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace kzn
