#pragma once

#include "device.hpp"

#include <vector>

namespace kzn::vk {
    
// TODO: Refactor this class for a simpler initialization
class RenderPass {
public:
    // Ctor
    RenderPass(
        Device& device,
        std::vector<VkAttachmentDescription>&& attachment_descriptions,
        std::vector<VkSubpassDescription>&& subpass_descriptions,
        std::vector<VkSubpassDependency>&& subpass_dependencies);
    // Copy
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    // Move
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;
    // Dtor
    ~RenderPass();

private:
    Device&      m_device;
    VkRenderPass m_render_pass;
};

} // namespace kzn::vk