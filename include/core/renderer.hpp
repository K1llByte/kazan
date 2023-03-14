#ifndef KZN_RENDERER_HPP
#define KZN_RENDERER_HPP

#include "core/window.hpp"
#include "core/context.hpp"
#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"
#include "vk/render_pass.hpp"
#include "vk/utils.hpp"

#include <functional>
#include <memory>

namespace kzn
{
    class PerFrameData
    {
    public:
        PerFrameData(vk::Device* _device, vk::CommandPool& cmd_pool)
            : cmd_buffer(cmd_pool.allocate())
        {
            PerFrameData::m_device = _device;
            img_available = vk::create_semaphore(*m_device);
            finished_render = vk::create_semaphore(*m_device);
            in_flight_fence = vk::create_fence(*m_device);
        }

        ~PerFrameData()
        {
            vk::destroy_semaphore(*m_device, img_available);
            vk::destroy_semaphore(*m_device, finished_render);
            vk::destroy_fence(*m_device, in_flight_fence);
        }

        // static std::size_t current() noexcept { return PerFrameData::next_idx; }
        static std::size_t next() noexcept { return (++PerFrameData::m_next_idx) % MAX_FRAMES_IN_FLIGHT; }

    public:
        static constexpr std::size_t MAX_FRAMES_IN_FLIGHT = 2;

        vk::CommandBuffer cmd_buffer;
        VkSemaphore       img_available;
        VkSemaphore       finished_render;
        VkFence           in_flight_fence;

    private:
        inline static std::size_t m_next_idx = 0;
        inline static vk::Device* m_device = nullptr;
    };


    class Renderer
    {
    public:
        Renderer(Window* window);
        ~Renderer();

        vk::CommandPool& cmd_pool() { return m_cmd_pool; }
        VkViewport& current_viewport() { return m_viewport; }
        VkRect2D& current_scissor() { return m_scissor; }

        virtual void render_frame(std::function<void(vk::CommandBuffer&)>);
        virtual const std::vector<VkImage>& get_render_images() { return Context::swapchain().images(); }
        virtual VkExtent2D get_render_extent() { return Context::swapchain().get_extent(); }

    protected:
        Window*                   m_window = nullptr;
        std::unique_ptr<Context>  m_context;
        vk::CommandPool           m_cmd_pool;
        std::vector<PerFrameData> m_per_frame_data;
        std::vector<VkFence>      m_image_fences;
        VkViewport                m_viewport;
        VkRect2D                  m_scissor;
    };
} // namespace kzn

#endif // KZN_RENDERER_HPP