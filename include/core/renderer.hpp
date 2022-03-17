#ifndef KZN_RENDERER_HPP
#define KZN_RENDERER_HPP

#include "core/window.hpp"
#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"
#include "vk/render_pass.hpp"
#include "vk/utils.hpp"

#include <functional>

namespace kzn
{
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

        // static std::size_t current() noexcept { return PerFrameData::next_idx; }
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

    // Forward declaration
    class ModelRederer;

    class Renderer
    {
        friend class ModelRenderer;
    public:
        Renderer(Window* window);
        ~Renderer();

        // NOTE: Only used within RenderSystem context 
        vk::CommandBuffer& current_cmd_buffer() { return *current_command_buffer; }
        VkViewport& current_viewport() { return viewport; }
        VkRect2D& current_scissor() { return scissor; }

        // NOTE: Only used to recreate RenderPass Framebuffers when
        // window is resized
        void add_render_pass(vk::RenderPass& render_pass);

        void render_frame(std::function<void(void)>);
    
    private:
        Window*         window = nullptr;
        vk::Instance    instance;
        VkSurfaceKHR    surface;
        vk::Device      device;
        vk::Swapchain   swapchain;
        vk::CommandPool cmd_pool;

        std::vector<PerFrameData> per_frame_data;
        std::vector<VkFence>      image_fences;
        VkViewport                viewport;
        VkRect2D                  scissor;
        vk::CommandBuffer*        current_command_buffer = nullptr;
        
        // RenderPasses that need recreation of framebuffers
        std::vector<vk::RenderPass*> render_passes;
    };
} // namespace kzn

#endif // KZN_RENDERER_HPP