#pragma once

#include "device.hpp"
#include "cmd_buffer.hpp"

#include <vector>
#include <span>
#include <functional>

namespace kzn::vk {

struct AttachmentDesc {
    VkAttachmentDescriptionFlags flags = {};
    VkFormat                     format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits        samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp           load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp          store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkAttachmentLoadOp           stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp          stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout                initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout                final_layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

using AttachmentRef = VkAttachmentReference;

struct SubpassDesc {
    VkSubpassDescriptionFlags  flags = {};
    VkPipelineBindPoint        pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<AttachmentRef> input_attachments;
    std::vector<AttachmentRef> color_attachments;
    std::vector<AttachmentRef> resolve_attachments;
    std::vector<AttachmentRef> depth_stencil_attachment;
    std::vector<uint32_t>      preserve_attachments;
};

struct RenderPassParams {
    std::vector<AttachmentDesc>      attachments;
    std::vector<SubpassDesc>         subpasses;
    std::vector<VkSubpassDependency> dependencies;
};

class Framebuffer;

class RenderPass {
public:
    // Ctor
    RenderPass(
        Device& device,
        RenderPassParams&& params);
    // Copy
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    // Move
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;
    // Dtor
    ~RenderPass();

    constexpr Device& device() { return m_device; }
    constexpr VkRenderPass vk_render_pass() const { return m_vk_render_pass; }

    void begin(CommandBuffer& cmd_buffer, Framebuffer& framebuffer, std::vector<VkClearValue>&& clear_values = {});
    void end(CommandBuffer& cmd_buffer);

private:
    Device&      m_device;
    VkRenderPass m_vk_render_pass;
    size_t       m_subpasses_count;
};

class Framebuffer {
public:
    // Ctor
    Framebuffer(
        RenderPass& render_pass,
        const std::vector<VkImageView>& attachments,
        VkExtent2D extent);
    // Copy
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    // Move
    Framebuffer(Framebuffer&&);
    Framebuffer& operator=(Framebuffer&&) = default;
    // Dtor
    ~Framebuffer();

    constexpr VkFramebuffer vk_framebuffer() const { return m_vk_framebuffer; }
    constexpr VkExtent2D extent() const { return m_extent; }
    
private:
    Device&       m_device;
    VkFramebuffer m_vk_framebuffer;
    VkExtent2D    m_extent;
};

} // namespace kzn::vk