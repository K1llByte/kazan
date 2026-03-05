#include "renderer.hpp"

#include "events/events.hpp"
#include "vk/utils.hpp"
#include <vulkan/vulkan_core.h>

namespace kzn {

PerFrameData::PerFrameData(vk::Device& device, vk::CommandPool& cmd_pool)
    : m_device_ptr{&device}
    , cmd_buffer(cmd_pool.allocate())
{
    image_available = vk::create_semaphore(*m_device_ptr);
    in_flight_fence =
        vk::create_fence(*m_device_ptr, VK_FENCE_CREATE_SIGNALED_BIT);
}

PerFrameData::PerFrameData(PerFrameData&& other)
    : cmd_buffer{std::move(other.cmd_buffer)}
    , image_available{other.image_available}
    , in_flight_fence{other.in_flight_fence}
    , m_device_ptr{other.m_device_ptr} {
    other.in_flight_fence = VK_NULL_HANDLE;
}

PerFrameData::~PerFrameData() {
    if (in_flight_fence != VK_NULL_HANDLE) {
        vk::destroy_semaphore(*m_device_ptr, image_available);
        vk::destroy_fence(*m_device_ptr, in_flight_fence);
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
          {
              .extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
              .features = {.wideLines = VK_TRUE, .samplerAnisotropy = VK_TRUE},
              .surface = m_surface,
          }
      )
    , m_swapchain(m_device, m_surface, window.extent())
    , m_cmd_pool(m_device) {
    // Initialize Per frame data
    m_frame_data.reserve(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        m_frame_data.emplace_back(device(), m_cmd_pool);
    }
    // Initialize render finished semaphores per swapchain image
    const auto images_count = m_swapchain.images().size();
    m_render_finished.reserve(m_swapchain.images().size());
    for(size_t i = 0; i < images_count; ++i) {
        m_render_finished.push_back(vk::create_semaphore(m_device));
    }
    // Set deleter queue expiration
    m_device.main_deletion_queue().set_expiration(MAX_FRAMES_IN_FLIGHT);
}

Renderer::~Renderer() {
    // Explicitly destroy semaphores
    for(const auto render_finished_sem : m_render_finished) {
        vk::destroy_semaphore(m_device, render_finished_sem);
    }
}

void Renderer::render_frame(const RenderFrameFn& render_func) {

    ///////////////////////////////////////////////////////////////////////////
    // Begin Frame
    ///////////////////////////////////////////////////////////////////////////

    auto& frame_data = m_frame_data[m_frame_idx];

    // Wait for previous frame
    vkWaitForFences(device(), 1, &frame_data.in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device(), 1, &frame_data.in_flight_fence);
    // After waiting fence we can delete expired deleters
    m_device.main_deletion_queue().flush_expired();

    // Acquire next frame
    auto opt_image_index = swapchain().acquire_next(frame_data.image_available);
    if (!opt_image_index.has_value()) {
        swapchain().recreate(m_window.extent());
        EventManager::send(SwapchainResizeEvent{});
        return;
    }
    uint32_t image_index = opt_image_index.value();

    // Only reset the fence if we are submitting work
    // vkResetFences(device(), 1, &frame_data.in_flight_fence);

    frame_data.cmd_buffer.reset();
    frame_data.cmd_buffer.begin();

    ///////////////////////////////////////////////////////////////////////////
    // Draw
    ///////////////////////////////////////////////////////////////////////////

    render_func(frame_data.cmd_buffer);

    ///////////////////////////////////////////////////////////////////////////
    // End Frame
    ///////////////////////////////////////////////////////////////////////////

    // Stop registering commands to the cmd buffer
    frame_data.cmd_buffer.end();

    // Submit command buffer
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &frame_data.image_available;
    submit_info.pWaitDstStageMask = wait_stages;
    VkCommandBuffer cmd_buffers[1] = {frame_data.cmd_buffer.vk_cmd_buffer()};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = cmd_buffers;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_render_finished[m_swapchain.current_index()];

    auto result = vkQueueSubmit(
        device().graphics_queue().vk_queue, 1, &submit_info, frame_data.in_flight_fence
    );
    VK_CHECK_MSG(result, "Failed to submit command buffer!");

    // Preset frame
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_render_finished[m_swapchain.current_index()];
    VkSwapchainKHR swapchains[] = {swapchain().vk_swapchain()};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;

    result =
        vkQueuePresentKHR(device().present_queue().vk_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_window.was_resized()) {
        swapchain().recreate(m_window.extent());
        EventManager::send(SwapchainResizeEvent{});
    }
    else if (result != VK_SUCCESS) {
        throw vk::ResultError(result);
    }

    // Increment frame in flight index
    m_frame_idx = (m_frame_idx + 1) % MAX_FRAMES_IN_FLIGHT;
    // Device deletion queue is basically a frame counter
    m_device.main_deletion_queue().increment_timer();
}

} // namespace kzn
