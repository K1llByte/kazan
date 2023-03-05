#include "core/passes/screen_pass.hpp"

#include "core/camera.hpp"
#include "core/model.hpp"

namespace kzn
{
    ScreenPass::ScreenPass()
        : EventHandlers{
            kzn::register_event_handler(this, &ScreenPass::on_resize)
        }
        , render_pass(vk::RenderPassBuilder(&Context::device())
            ///////// Specify attachments descriptions /////////
            .add_attachment(
                vk::AttachmentDesc(Context::swapchain().get_surface_format().format)
                    .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL // AttachmentRef layout
            )
            .add_attachment(
                vk::AttachmentDesc(VK_FORMAT_D32_SFLOAT)
                    .set_stencil_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR) // NOT NEEDED
                    .set_final_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL),
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // AttachmentRef layout
            )
            ///////// Specify subpasses /////////
            .add_subpass(
                vk::SubpassDesc(VK_PIPELINE_BIND_POINT_GRAPHICS)
                    .set_color_attachments({0})
                    .set_depth_stencil_attachment(1)
            )
            .add_subpass_dependency(
                // Don't know what's this yet
                VkSubpassDependency{
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                }
            )
            .add_subpass_dependency(
                // Depth test dependency
                VkSubpassDependency{
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                }
            ).build()
        )
        // Initialize framebuffers
        , framebuffers(
            &Context::device(),
            &render_pass,
            Context::swapchain().images(),
            Context::swapchain().get_extent())
    {

    }

    void ScreenPass::on_resize(const ResizeEvent&) {
        framebuffers.recreate(
            Context::swapchain().images(),
            Context::swapchain().get_extent()
        );
    }
} // namespace kzn