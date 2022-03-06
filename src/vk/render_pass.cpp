#include "vk/render_pass.hpp"

#include "vk/utils.hpp"

namespace kzn::vk
{
    RenderPassBuilder::RenderPassBuilder(Device* device)
        : device(device) {}

    RenderPassBuilder& RenderPassBuilder::set_format(VkFormat format) noexcept
    {
        color_format = format;
        return *this;
    }

    RenderPass RenderPassBuilder::build()
    {
        // REQUIRES: SwapChain's VkFormat 
        VkAttachmentDescription color_attachment{};
        color_attachment.format = color_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        // TODO: For now this subpass is hardcoded, but
        // in the future this builder must have methods to add
        // and configure more subpasses
        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPass vkrender_pass;
        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        auto result = vkCreateRenderPass(device->vk_device(), &render_pass_info, nullptr, &vkrender_pass);
        VK_CHECK_MSG(result, "Failed to create render pass!");
        Log::debug("RenderPass created");

        auto render_pass = RenderPass();
        render_pass.device = device;
        render_pass.vkrender_pass = vkrender_pass;
        return render_pass;
    }
    
    RenderPass::~RenderPass()
    {
        vkDestroyRenderPass(device->vk_device(), vkrender_pass, nullptr);
        Log::debug("RenderPass destroyed");
    }
}