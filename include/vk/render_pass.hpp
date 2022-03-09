#ifndef KZN_VK_RENDER_PASS_HPP
#define KZN_VK_RENDER_PASS_HPP

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "vk/cmd_buffers.hpp"

namespace kzn::vk
{
    class RenderPass
    {
        friend class RenderPassBuilder;
    public:
        ~RenderPass();
    
        VkRenderPass vk_render_pass() noexcept { return vkrender_pass; }

        void create_framebuffers(Swapchain& swapchain);
        void begin(CommandBuffer& cmd_buffer, const Swapchain& swapchain);
        void end(CommandBuffer& cmd_buffer);

    private:
        RenderPass() = default;

    private:
        VkRenderPass               vkrender_pass;
        Device*                    device;
        std::vector<VkFramebuffer> framebuffers;
    };

    class RenderPassBuilder
    {
    public:
        RenderPassBuilder(Device* device);

        // TODO: Change this to a configurable way to setup
        // many attachments and subpasses 
        RenderPassBuilder& set_format(VkFormat format) noexcept;

        RenderPass build();

    private:
        Device*  device;
        VkFormat color_format;
    };
}

#endif // KZN_VK_RENDER_PASS_HPP