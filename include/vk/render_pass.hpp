#ifndef KZN_VK_RENDER_PASS_HPP
#define KZN_VK_RENDER_PASS_HPP

#include "vk/device.hpp"

namespace kzn::vk
{
    class RenderPass
    {
        friend class RenderPassBuilder;
    public:
        ~RenderPass();
    
        VkRenderPass vk_render_pass() noexcept { return vkrender_pass; }

    private:
        RenderPass() = default;

    private:
        VkRenderPass vkrender_pass;
        Device*      device;
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