#include "utils.hpp"

namespace kzn {

std::vector<vk::Framebuffer> create_swapchain_framebuffers(
    vk::RenderPass& render_pass,
    vk::Swapchain& swapchain)
{
    std::vector<vk::Framebuffer> framebuffers;
    const size_t imgs_count = swapchain.image_views().size();
    framebuffers.reserve(imgs_count);

    for(size_t i = 0; i < imgs_count; ++i) {
        auto image_views = std::vector{ swapchain.image_views()[i] };
        framebuffers.emplace_back(
            render_pass,
            std::move(image_views),
            swapchain.extent()
        );
    }

    return framebuffers;
}


vk::RenderPass simple_pass(vk::Device& device, VkFormat color_format) {
    std::vector attachment_descs {
        vk::AttachmentDesc{
            .format = color_format,
            .load_op = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .store_op = VK_ATTACHMENT_STORE_OP_STORE,
            .final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
    };
    auto color_ref = vk::AttachmentRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    return vk::RenderPass(
        device,
        vk::RenderPassParams{
            .attachments = attachment_descs,
            .subpasses = {
                vk::SubpassDesc{
                    .color_attachments = { color_ref },
                }
            },
            .dependencies = {
                VkSubpassDependency{
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                }
            }
        }
    );
}

} // namespace kzn
