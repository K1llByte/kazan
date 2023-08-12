#include "render_pass.hpp"

#include "vk/error.hpp"

namespace kzn::vk {

RenderPass::RenderPass(
    Device& device,
    std::vector<VkAttachmentDescription>&& attachment_descriptions,
    std::vector<VkSubpassDescription>&& subpass_descriptions,
    std::vector<VkSubpassDependency>&& subpass_dependencies)
    : m_device{device}
{
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
    render_pass_info.pAttachments = attachment_descriptions.data();
    render_pass_info.subpassCount = static_cast<uint32_t>(subpass_descriptions.size());
    render_pass_info.pSubpasses = subpass_descriptions.data();
    render_pass_info.dependencyCount = static_cast<uint32_t>(subpass_dependencies.size());
    render_pass_info.pDependencies = subpass_dependencies.data();

    auto result = vkCreateRenderPass(m_device.vk_device(), &render_pass_info, nullptr, &m_render_pass);
    VK_CHECK_MSG(result, "Failed to create render pass!");
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(m_device.vk_device(), m_render_pass, nullptr);
}
    
} // namespace kzn::vk