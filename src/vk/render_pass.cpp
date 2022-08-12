#include "vk/render_pass.hpp"

#include "vk/utils.hpp"
#include "core/log.hpp"


// TODO: Delete
// void destroy_framebuffers(kzn::vk::Device* device, const std::vector<VkFramebuffer>& framebuffers)
// {
//     for(const auto& framebuffer : framebuffers) {
//         vkDestroyFramebuffer(device->vk_device(), framebuffer, nullptr);
//     }
// }

namespace kzn::vk
{
    RenderPass RenderPassBuilder::build() {
        // Build all VkSubpassDescription's
        std::vector<VkSubpassDescription> real_subpass_descriptions;
        real_subpass_descriptions.reserve(subpasses_descriptions.size());
        // NOTE: The RenderPassBuilder will be limited to only 32 attachments in total
        // for all subpasses (this way, there's only 1 dynamic allocation for the conversion)
        std::vector<VkAttachmentReference> tmp_attachment_desc_storage;
        tmp_attachment_desc_storage.reserve(32);

        for(const auto& desc : subpasses_descriptions) {
            // Convert input_attachments into vector
            // of VkAttachmentReference's
            const auto real_input_attachments_pos =
                tmp_attachment_desc_storage.size();
            for(size_t i = 0; i < desc.input_attachments.size(); ++i) {
                if(desc.input_attachments[i] > attachment_refs.size()-1)
                    throw InvalidAttachmentIndex();

                tmp_attachment_desc_storage.push_back(attachment_refs[desc.input_attachments[i]]);
            }

            // Convert color_attachments into dyn array
            // of VkAttachmentReference's
            const auto real_color_attachments_pos =
                tmp_attachment_desc_storage.size();
            for(size_t i = 0; i < desc.color_attachments.size(); ++i) {
                if(desc.color_attachments[i] > attachment_refs.size()-1)
                    throw InvalidAttachmentIndex();

                tmp_attachment_desc_storage.push_back(attachment_refs[desc.color_attachments[i]]);
            }

            if(desc.resolve_attachments.size() != 0
                && desc.resolve_attachments.size() != desc.color_attachments.size())
                    throw ColorAndResolveNotSameSize();

            // Convert resolve_attachments into dyn array
            // of VkAttachmentReference's
            const auto real_resolve_attachments_pos = tmp_attachment_desc_storage.size();
                // desc.resolve_attachments.size()
                //     ? tmp_attachment_desc_storage.data() + tmp_attachment_desc_storage.size()
                //     : nullptr;

            for(size_t i = 0; i < desc.resolve_attachments.size(); ++i) {
                if(desc.resolve_attachments[i] > attachment_refs.size()-1)
                    throw InvalidAttachmentIndex();

                tmp_attachment_desc_storage.push_back(attachment_refs[desc.resolve_attachments[i]]);
            }

            real_subpass_descriptions.push_back(
                VkSubpassDescription{
                    .flags = desc.flags,
                    .pipelineBindPoint = desc.bind_point,
                    .inputAttachmentCount = static_cast<uint32_t>(desc.input_attachments.size()),
                    .pInputAttachments = tmp_attachment_desc_storage.data() + real_input_attachments_pos,
                    .colorAttachmentCount = static_cast<uint32_t>(desc.color_attachments.size()),
                    .pColorAttachments = tmp_attachment_desc_storage.data() + real_color_attachments_pos,
                    .pResolveAttachments = desc.resolve_attachments.size()
                        ? tmp_attachment_desc_storage.data() + real_resolve_attachments_pos
                        : nullptr,
                    .pDepthStencilAttachment = (desc.depth_stencil_attachment == size_t(-1))
                        ? nullptr
                        : &attachment_refs[desc.depth_stencil_attachment],
                    .preserveAttachmentCount = static_cast<uint32_t>(desc.preserve_attachments.size()),
                    .pPreserveAttachments = desc.preserve_attachments.data(),
                }
            );
        }

        auto render_pass_create_info = VkRenderPassCreateInfo {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            // .pNext = nullptr,
            // .flags = 0,
            .attachmentCount = static_cast<uint32_t>(attachments_descriptions.size()),
            .pAttachments = attachments_descriptions.data(),
            .subpassCount = static_cast<uint32_t>(real_subpass_descriptions.size()),
            .pSubpasses = real_subpass_descriptions.data(),
            .dependencyCount = static_cast<uint32_t>(subpasses_dependencies.size()),
            .pDependencies = subpasses_dependencies.data(),
        };

        VkRenderPass vkrender_pass;
        auto result = vkCreateRenderPass(device->vk_device(), &render_pass_create_info, nullptr, &vkrender_pass);
        VK_CHECK_MSG(result, "Failed to create render pass!");
        Log::debug("RenderPass created");

        return RenderPass(device, vkrender_pass);
    }
    

    RenderPass::~RenderPass()
    {
        // TODO: Delete
        // Destroy Framebuffers
        // destroy_framebuffers(device, framebuffers);
        // Log::debug("Framebuffers destroyed");
        // Destroy RenderPass
        vkDestroyRenderPass(device->vk_device(), vkrender_pass, nullptr);
        Log::debug("RenderPass destroyed");
    }


    // void RenderPass::create_framebuffers(Swapchain& swapchain)
    // {
    //     const auto num_images = swapchain.num_images();
    //     framebuffers.resize(num_images);
    //     auto& image_views = swapchain.image_views();
    //     // Its a vector, but can be just 1 depth buffer
    //     auto& depth_image_views = swapchain.depth_image_views();
    //     auto extent = swapchain.get_extent();
    //     for(std::size_t i = 0; i < num_images; ++i)
    //     {
    //         auto attachments = std::array{
    //             image_views[i],
    //             depth_image_views[0] // [i]
    //         };

    //         VkFramebufferCreateInfo create_info{};
    //         create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    //         create_info.renderPass = vkrender_pass;
    //         create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    //         create_info.pAttachments = attachments.data();
    //         create_info.width = extent.width;
    //         create_info.height = extent.height;
    //         create_info.layers = 1;

    //         auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffers[i]);
    //         VK_CHECK_MSG(result, "Failed to create framebuffer!");
    //     }
    //     Log::debug("Framebuffers created");
    // }


    // void RenderPass::recreate_framebuffers(Swapchain& swapchain)
    // {
    //     auto num_images = framebuffers.size();
    //     // Destroy old framebuffers
    //     destroy_framebuffers(device, framebuffers);
    //     // Create new VkFramebuffers
    //     auto& image_views = swapchain.image_views();
    //     auto& depth_image_views = swapchain.depth_image_views();
    //     auto extent = swapchain.get_extent();
    //     for(std::size_t i = 0; i < num_images; ++i)
    //     {
    //         auto attachments = std::array{
    //             image_views[i],
    //             depth_image_views[0]
    //         };

    //         VkFramebufferCreateInfo create_info{};
    //         create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    //         create_info.renderPass = vkrender_pass;
    //         create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    //         create_info.pAttachments = attachments.data();
    //         create_info.width = extent.width;
    //         create_info.height = extent.height;
    //         create_info.layers = 1;

    //         auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffers[i]);
    //         VK_CHECK_MSG(result, "Failed to create framebuffer!");
    //     }
    //     Log::debug("Framebuffers recreated");
    // }


    void RenderPass::begin(CommandBuffer& cmd_buffer, Framebuffer& framebuffer)
    {
        VkRenderPassBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin_info.renderPass = vkrender_pass;
        begin_info.framebuffer = framebuffer.vk_framebuffer;
        begin_info.renderArea.offset = {0, 0};
        begin_info.renderArea.extent = framebuffer.extent;

        // VkClearValue depth_clear;
        // depth_clear.depthStencil.depth = 1.f;
        // auto clear_values = std::array{
        //     /* Color clear */
        //     VkClearValue{{{0.009, 0.009, 0.009, 1.0f}}},
        //     /* Depth clear */
        //     depth_clear
        //     // VkClearValue{{1.f, 0.f}}
        // };

        begin_info.clearValueCount = static_cast<uint32_t>(framebuffer.clear_values.size());
        begin_info.pClearValues = framebuffer.clear_values.data();

        // VK_SUBPASS_CONTENTS_INLINE: The render pass commands
        // will be embedded in the primary command buffer itself and
        // no secondary command buffers will be executed.
        // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render
        // pass commands will be executed from secondary command buffers.
        vkCmdBeginRenderPass(cmd_buffer.vk_command_buffer(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }


    void RenderPass::end(CommandBuffer& cmd_buffer)
    {
        vkCmdEndRenderPass(cmd_buffer.vk_command_buffer());
    }


    RenderPass::RenderPass(Device* _device, VkRenderPass _render_pass)
        : device(_device), vkrender_pass(_render_pass) {}

    //////////////////////// FIXME: TEMPORARY ///////////////////////////////

    RenderPass simple_depth_render_pass(vk::Device& device, VkFormat surface_format) {
        auto render_pass_builder = RenderPassBuilder(&device)
            ///////// Specify attachments descriptions /////////
            .add_attachment(
                vk::AttachmentDesc(surface_format)
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
            );
        return render_pass_builder.build();
    }
}