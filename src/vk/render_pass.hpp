#pragma once

#include "cmd_buffer.hpp"
#include "core/log.hpp"
#include "device.hpp"
#include "error.hpp"

#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace kzn::vk {

struct AttachmentDesc {
    VkAttachmentDescriptionFlags flags = {};
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkAttachmentLoadOp stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

using AttachmentRef = VkAttachmentReference;

struct SubpassDesc {
    VkSubpassDescriptionFlags flags = {};
    VkPipelineBindPoint pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<AttachmentRef> input_attachments;
    std::vector<AttachmentRef> color_attachments;
    std::vector<AttachmentRef> resolve_attachments;
    std::vector<AttachmentRef> depth_stencil_attachment;
    std::vector<uint32_t> preserve_attachments;
};

struct RenderPassParams {
    std::vector<AttachmentDesc> attachments;
    std::vector<SubpassDesc> subpasses;
    std::vector<VkSubpassDependency> dependencies;
};

class Framebuffer;

class RenderPass {
public:
    // Ctor
    RenderPass(Device& device, RenderPassParams&& params);
    // Copy
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    // Move
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;
    // Dtor
    ~RenderPass();

    constexpr Device& device() { return m_device; }

    operator VkRenderPass() const { return m_vk_render_pass; }

    void begin(
        CommandBuffer& cmd_buffer,
        Framebuffer& framebuffer,
        std::vector<VkClearValue>&& clear_values = {}
    );
    void end(CommandBuffer& cmd_buffer);

private:
    Device& m_device;
    VkRenderPass m_vk_render_pass;
    size_t m_subpasses_count;
};

class Framebuffer {
public:
    // Ctor
    template<typename C>
        requires std::ranges::contiguous_range<C> && std::ranges::sized_range<C>
    Framebuffer(
        RenderPass& render_pass,
        const C& attachments,
        VkExtent2D extent
    );
    // Copy
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    // Move
    Framebuffer(Framebuffer&&);
    Framebuffer& operator=(Framebuffer&&);
    // Dtor
    ~Framebuffer();

    [[nodiscard]]
    constexpr VkFramebuffer vk_framebuffer() const {
        return m_vk_framebuffer;
    }
    [[nodiscard]]
    constexpr VkExtent2D extent() const {
        return m_extent;
    }

private:
    Device& m_device;
    VkFramebuffer m_vk_framebuffer = VK_NULL_HANDLE;
    VkExtent2D m_extent;
};

/////////////////////////////////////////////////////////////////

template<typename C>
    requires std::ranges::contiguous_range<C> && std::ranges::sized_range<C>
Framebuffer::Framebuffer(
    RenderPass& render_pass,
    const C& attachments,
    VkExtent2D extent
)
    : m_device{render_pass.device()}
    , m_extent{extent} {

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass;
    framebuffer_info.attachmentCount = std::ranges::size(attachments);
    framebuffer_info.pAttachments = std::ranges::data(attachments);
    framebuffer_info.width = extent.width;
    framebuffer_info.height = extent.height;
    framebuffer_info.layers = 1;

    auto result = vkCreateFramebuffer(
        m_device, &framebuffer_info, nullptr, &m_vk_framebuffer
    );
    VK_CHECK_MSG(result, "Failed to create framebuffer!");
}

} // namespace kzn::vk