#include "vk/render_pass.hpp"

#include "vk/utils.hpp"
#include "core/log.hpp"


void destroy_framebuffers(kzn::vk::Device* device, const std::vector<VkFramebuffer>& framebuffers)
{
    for(const auto& framebuffer : framebuffers) {
        vkDestroyFramebuffer(device->vk_device(), framebuffer, nullptr);
    }
}

namespace kzn::vk
{
    // RenderPassBuilder& RenderPassBuilder::set_format(VkFormat format) noexcept
    // {
    //     color_format = format;
    //     return *this;
    // }

    RenderPass RenderPassBuilder::build() {        
        // Build all VkSubpassDescription's
        std::vector<VkSubpassDescription> real_subpass_descriptions;
        real_subpass_descriptions.reserve(subpasses_descriptions.size());
        // NOTE: The RenderPassBuilder will be limited to only 32 attachments in total
        // for all subpasse (this way, there's only 1 dynamic allocation for the conversion)
        std::vector<VkAttachmentReference> tmp_attachment_desc_storage;
        tmp_attachment_desc_storage.reserve(32);

        for(const auto& desc : subpasses_descriptions) {
            // Convert input_attachments into vector
            // of VkAttachmentReference's
            const auto* real_input_attachments =
                tmp_attachment_desc_storage.data() + tmp_attachment_desc_storage.size();
            for(size_t i = 0; i < desc.input_attachments.size(); ++i) {
                // TODO: error handling if index is out of bounds of attachment_refs
                tmp_attachment_desc_storage.push_back(attachment_refs[desc.input_attachments[i]]);
            }

            // Convert color_attachments into dyn array
            // of VkAttachmentReference's
            const auto* real_color_attachments =
                tmp_attachment_desc_storage.data() + tmp_attachment_desc_storage.size();
            for(size_t i = 0; i < desc.color_attachments.size(); ++i) {
                // TODO: error handling if index is out of bounds of attachment_refs
                tmp_attachment_desc_storage.push_back(attachment_refs[desc.color_attachments[i]]);
            }

            if(desc.resolve_attachments.size() != 0
                && desc.resolve_attachments.size() != desc.color_attachments.size())
            {
                // TODO: Handle this error (they must be of the same size)
            }
            Log::error("Resolve attachments size: {}", desc.resolve_attachments.size());
            // Convert resolve_attachments into dyn array
            // of VkAttachmentReference's
            const auto* real_resolve_attachments =
                desc.resolve_attachments.size()
                    ? tmp_attachment_desc_storage.data() + tmp_attachment_desc_storage.size()
                    : nullptr;

            for(size_t i = 0; i < desc.resolve_attachments.size(); ++i) {
                // TODO: error handling if index is out of bounds of attachment_refs
                tmp_attachment_desc_storage.push_back(attachment_refs[desc.resolve_attachments[i]]);
            }

            real_subpass_descriptions.push_back(
                VkSubpassDescription{
                    .flags = desc.flags,
                    .pipelineBindPoint = desc.bind_point,
                    .inputAttachmentCount = static_cast<uint32_t>(desc.input_attachments.size()),
                    .pInputAttachments = real_input_attachments,
                    .colorAttachmentCount = static_cast<uint32_t>(desc.color_attachments.size()),
                    .pColorAttachments = real_color_attachments,
                    .pResolveAttachments = real_resolve_attachments,
                    .pDepthStencilAttachment = &attachment_refs[desc.depth_stencil_attachment],
                    .preserveAttachmentCount = static_cast<uint32_t>(desc.preserve_attachments.size()),
                    .pPreserveAttachments = desc.preserve_attachments.data(),
                }
            );
        }

        // attachments_descriptions
        // real_subpass_descriptions
        // subpasses_dependencies
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

        // TODO: return RenderPass(device, vkrender_pass)
        auto render_pass = RenderPass();
        render_pass.device = device;
        render_pass.vkrender_pass = vkrender_pass;
        return render_pass;
    }
    // RenderPass RenderPassBuilder::build()
    // {
    //     // REQUIRES: SwapChain's VkFormat 
    //     VkAttachmentDescription color_attachment{};
    //     color_attachment.format = color_format;
    //     color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //     color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //     color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //     color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //     color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //     color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //     VkAttachmentDescription depth_attachment{};
    //     depth_attachment.format = VK_FORMAT_D32_SFLOAT;
    //     depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //     depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //     depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //     depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // VK_ATTACHMENT_LOAD_OP_DONT_CARE works too (i think)
    //     depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //     depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //     depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    //     // TODO: For now this subpass is hardcoded, but
    //     // in the future this builder must have methods to add
    //     // and configure more subpasses
    //     VkAttachmentReference color_attachment_ref{};
    //     color_attachment_ref.attachment = 0;
    //     color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //     VkAttachmentReference depth_attachment_ref = {};
    //     depth_attachment_ref.attachment = 1;
    //     depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //     // Subpasses descriptions //
    //     auto attachments = std::array{color_attachment, depth_attachment};

    //     VkSubpassDescription subpass{};
    //     subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    //     subpass.colorAttachmentCount = 1;
    //     subpass.pColorAttachments = &color_attachment_ref;
    //     subpass.pDepthStencilAttachment = &depth_attachment_ref;

    //     VkRenderPass vkrender_pass;
    //     VkRenderPassCreateInfo render_pass_info{};
    //     render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    //     render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    //     render_pass_info.pAttachments = attachments.data();
    //     render_pass_info.subpassCount = 1;
    //     render_pass_info.pSubpasses = &subpass;

    //     // Subpass dependencies
    //     VkSubpassDependency dependency{};
    //     dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    //     dependency.dstSubpass = 0;
    //     dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //     dependency.srcAccessMask = 0;
    //     dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //     dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //     // TODO: Learn more about this
    //     VkSubpassDependency depth_dependency{};
    //     depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    //     depth_dependency.dstSubpass = 0;
    //     depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    //     depth_dependency.srcAccessMask = 0;
    //     depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    //     depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    //     auto dependencies = std::array{ dependency, depth_dependency };

    //     render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
    //     render_pass_info.pDependencies = dependencies.data();


    //     auto result = vkCreateRenderPass(device->vk_device(), &render_pass_info, nullptr, &vkrender_pass);
    //     VK_CHECK_MSG(result, "Failed to create render pass!");
    //     Log::debug("RenderPass created");

    //     auto render_pass = RenderPass();
    //     render_pass.device = device;
    //     render_pass.vkrender_pass = vkrender_pass;
    //     return render_pass;
    // }

    

    RenderPass::~RenderPass()
    {
        // Destroy Framebuffers
        destroy_framebuffers(device, framebuffers);
        Log::debug("Framebuffers destroyed");
        // Destroy RenderPass
        vkDestroyRenderPass(device->vk_device(), vkrender_pass, nullptr);
        Log::debug("RenderPass destroyed");
    }

    void RenderPass::create_framebuffers(Swapchain& swapchain)
    {
        const auto num_images = swapchain.num_images();
        framebuffers.resize(num_images);
        auto& image_views = swapchain.image_views();
        auto& depth_image_views = swapchain.depth_image_views();
        auto extent = swapchain.get_extent();
        for(std::size_t i = 0; i < num_images; ++i)
        {
            // TODO: Change from multiple depth buffers to only 1
            auto attachments = std::array{
                image_views[i],
                depth_image_views[i]
            };

            VkFramebufferCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            create_info.renderPass = vkrender_pass;
            create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            create_info.pAttachments = attachments.data();
            create_info.width = extent.width;
            create_info.height = extent.height;
            create_info.layers = 1;

            auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffers[i]);
            VK_CHECK_MSG(result, "Failed to create framebuffer!");
        }
        Log::debug("Framebuffers created");
    }

    void RenderPass::recreate_framebuffers(Swapchain& swapchain)
    {
        auto num_images = framebuffers.size();
        // Destroy old framebuffers
        destroy_framebuffers(device, framebuffers);
        // Create new VkFramebuffers
        auto& image_views = swapchain.image_views();
        auto& depth_image_views = swapchain.depth_image_views();
        auto extent = swapchain.get_extent();
        for(std::size_t i = 0; i < num_images; ++i)
        {
            auto attachments = std::array{
                image_views[i],
                depth_image_views[i]
            };

            VkFramebufferCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            create_info.renderPass = vkrender_pass;
            create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            create_info.pAttachments = attachments.data();
            create_info.width = extent.width;
            create_info.height = extent.height;
            create_info.layers = 1;

            auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffers[i]);
            VK_CHECK_MSG(result, "Failed to create framebuffer!");
        }
        Log::debug("Framebuffers recreated");
    }

    void RenderPass::begin(CommandBuffer& cmd_buffer, const Swapchain& swapchain)
    {
        VkRenderPassBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin_info.renderPass = vkrender_pass;
        begin_info.framebuffer = framebuffers[swapchain.current_index()];
        begin_info.renderArea.offset = {0, 0};
        auto ext = swapchain.get_extent();

        begin_info.renderArea.extent = ext;

        VkClearValue depth_clear;
	    depth_clear.depthStencil.depth = 1.f;
        auto clear_values = std::array{
            /* clear_color */
            VkClearValue{{{0.160f, 0.156f, 0.156f, 1.0f}}},
            depth_clear
            // VkClearValue{{1.f, 0.f}}
        };
        begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        begin_info.pClearValues = clear_values.data();

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