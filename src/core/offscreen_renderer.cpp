#include "core/offscreen_renderer.hpp"

#include "vk/error.hpp"

namespace kzn {
    OffscreenRenderer::OffscreenRenderer(Window* _window, ImGuiWindow* _imgui_window)
        : Renderer(_window)
        , imgui_window(_imgui_window)
        , render_images(3)
        , render_allocation(3)
    {
        const auto num_images = Context::swapchain().num_images();
        render_extent = Context::swapchain().get_extent();
        for(size_t i = 0; i < num_images; ++i) {
            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.format = VK_FORMAT_B8G8R8A8_SRGB;
            image_info.extent = VkExtent3D{
                render_extent.width,
                render_extent.height,
                1
            };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            // image_info.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // Allocate it from GPU local memory
            VmaAllocationCreateInfo img_alloc_info{};
            img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            auto result = vmaCreateImage(Context::device().allocator(), &image_info, &img_alloc_info, &render_images[i], &render_allocation[i], nullptr);
            // TODO: Uncomment
            // VK_CHECK_MSG(result, "Failed to create color image!");
        }
    }

    
    OffscreenRenderer::~OffscreenRenderer() {
        // Destroy render images
        for(size_t i = 0; i < render_images.size(); ++i) {
            vmaDestroyImage(Context::device().allocator(), render_images[i], render_allocation[i]);
        }
    }


    void OffscreenRenderer::render_frame(std::function<void(vk::CommandBuffer&)> draw_func) {
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