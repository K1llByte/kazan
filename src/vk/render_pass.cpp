#include "render_pass.hpp"

#include "vk/error.hpp"
#include "core/log.hpp"

#include <algorithm>

namespace kzn::vk {

RenderPass::RenderPass(
    Device& device,
    RenderPassParams&& params)
    : m_device{device}
    , m_subpasses_count{params.subpasses.size()}
{
    // Convert params.attachments
    std::vector<VkAttachmentDescription> attachments;
    std::ranges::transform(params.attachments, std::back_inserter(attachments),
        [](const AttachmentDesc& ad) {
            return VkAttachmentDescription{
                .flags = ad.flags,
                .format = ad.format,
                .samples = ad.samples,
                .loadOp = ad.load_op,
                .storeOp = ad.store_op,
                .stencilLoadOp = ad.stencil_load_op,
                .stencilStoreOp = ad.stencil_store_op,
                .initialLayout = ad.initial_layout,
                .finalLayout = ad.final_layout,
            };
        }
    );

    // CONVERT: params.subpasses
    std::vector<VkSubpassDescription> subpasses;
    std::ranges::transform(params.subpasses, std::back_inserter(subpasses),
        [](const SubpassDesc& sd) {
            return VkSubpassDescription {
                .flags = sd.flags,
                .pipelineBindPoint = sd.pipeline_bind_point,
                .inputAttachmentCount = static_cast<uint32_t>(sd.input_attachments.size()),
                .pInputAttachments = sd.input_attachments.data(),
                .colorAttachmentCount = static_cast<uint32_t>(sd.color_attachments.size()),
                .pColorAttachments = sd.color_attachments.data(),
                .pResolveAttachments = sd.resolve_attachments.data(),
                .pDepthStencilAttachment = sd.depth_stencil_attachment.data(),
                .preserveAttachmentCount = static_cast<uint32_t>(sd.preserve_attachments.size()),
                .pPreserveAttachments = sd.preserve_attachments.data(),
            };
        }
    );


    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = static_cast<uint32_t>(subpasses.size());
    render_pass_info.pSubpasses = subpasses.data();
    render_pass_info.dependencyCount = static_cast<uint32_t>(params.dependencies.size());
    render_pass_info.pDependencies = params.dependencies.data();

    auto result = vkCreateRenderPass(m_device.vk_device(), &render_pass_info, nullptr, &m_vk_render_pass);
    VK_CHECK_MSG(result, "Failed to create render pass!");
    Log::trace("Render Pass created");
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(m_device.vk_device(), m_vk_render_pass, nullptr);
    Log::trace("Render Pass destroyed");
}


void RenderPass::begin(CommandBuffer& cmd_buffer, Framebuffer& framebuffer, std::vector<VkClearValue>&& clear_values) {
    VkRenderPassBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.renderPass = m_vk_render_pass;
    begin_info.framebuffer = framebuffer.vk_framebuffer();
    begin_info.renderArea.offset = {0, 0};
    begin_info.renderArea.extent = framebuffer.extent();

    begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    begin_info.pClearValues = clear_values.data();

    // VK_SUBPASS_CONTENTS_INLINE: The render pass commands
    // will be embedded in the primary command buffer itself and
    // no secondary command buffers will be executed.
    // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render
    // pass commands will be executed from secondary command buffers.
    vkCmdBeginRenderPass(cmd_buffer.vk_cmd_buffer(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}


void RenderPass::end(CommandBuffer& cmd_buffer) {
    vkCmdEndRenderPass(cmd_buffer.vk_cmd_buffer());
}


Framebuffer::Framebuffer(
    RenderPass& render_pass,
    const std::vector<VkImageView>& attachments,
    VkExtent2D extent)
    : m_device{render_pass.device()}
    , m_extent{extent}
{
    
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass.vk_render_pass();
    framebuffer_info.attachmentCount = attachments.size();
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = extent.width;
    framebuffer_info.height = extent.height;
    framebuffer_info.layers = 1;

    auto result = vkCreateFramebuffer(m_device.vk_device(), &framebuffer_info, nullptr, &m_vk_framebuffer);
    VK_CHECK_MSG(result, "Failed to create framebuffer!");
}

Framebuffer::Framebuffer(Framebuffer&& other)
    : m_device{other.m_device}
    , m_vk_framebuffer{other.m_vk_framebuffer}
{
    other.m_vk_framebuffer = VK_NULL_HANDLE;
}

Framebuffer::~Framebuffer() {
    if(m_vk_framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device.vk_device(), m_vk_framebuffer, nullptr);
    }
}
} // namespace kzn::vk