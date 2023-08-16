#include "renderer.hpp"

#include "vk/utils.hpp"

namespace kzn {

Renderer::Renderer(
    vk::Device& device,
    vk::Swapchain& swapchain,
    Window& window)
    : m_device{device}
    , m_swapchain{swapchain}
    , m_cmd_pool(device)
    , m_cmd_buffer(m_cmd_pool.allocate())
    , m_window{window}
{
    m_image_available = vk::create_semaphore(m_device);
    m_render_finished = vk::create_semaphore(m_device);
    m_in_flight_fence = vk::create_fence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
}


Renderer::~Renderer() {
    vk::destroy_semaphore(m_device, m_image_available);
    vk::destroy_semaphore(m_device, m_render_finished);
    vk::destroy_fence(m_device, m_in_flight_fence);
}


void Renderer::render_frame(const RenderFrameFunc& render_func) {

    /////////////////
    // Begin Frame //
    /////////////////

    // Wait for previous frame
    vkWaitForFences(m_device.vk_device(), 1, &m_in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.vk_device(), 1, &m_in_flight_fence);

    // Acquire next frame
    // TODO: Turn this into vk::Swapchain member function
    uint32_t imageIndex;
    vkAcquireNextImageKHR(
        m_device.vk_device(),
        m_swapchain.vk_swapchain(),
        UINT64_MAX,
        m_image_available,
        VK_NULL_HANDLE,
        &imageIndex);
    
    m_cmd_buffer.reset();
    m_cmd_buffer.begin();

    /////////////////
    //    Draw     //
    /////////////////

    render_func(m_cmd_buffer);

    /////////////////
    //  End Frame  //
    /////////////////

    // Stop registering commands to the cmd buffer
    m_cmd_buffer.end();

    // Submit command buffer
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore wait_semaphores[] = { m_image_available };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    auto cmd_buffer = m_cmd_buffer.vk_cmd_buffer();
    submit_info.pCommandBuffers = &cmd_buffer;
    VkSemaphore signal_semaphores[] = { m_render_finished }; 
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    auto result = vkQueueSubmit(m_device.graphics_queue() , 1, &submit_info, m_in_flight_fence);
    VK_CHECK_MSG(result, "Failed to submit command buffer!");
    
    // Preset frame
}

} // namespace kzn
