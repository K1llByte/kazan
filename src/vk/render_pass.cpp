#include "vk/render_pass.hpp"

#include "vk/utils.hpp"
#include "core/log.hpp"


void destroy_framebuffers(kzn::vk::Device* device, const std::vector<VkFramebuffer>& framebuffers)
{
    for(const auto& framebuffer : framebuffers)
    {
        vkDestroyFramebuffer(device->vk_device(), framebuffer, nullptr);
    }
}

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

        VkAttachmentDescription depth_attachment{};
        depth_attachment.format = VK_FORMAT_D32_SFLOAT;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


        // TODO: For now this subpass is hardcoded, but
        // in the future this builder must have methods to add
        // and configure more subpasses
        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref = {};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Subpasses descriptions //
        auto attachments = std::array{color_attachment, depth_attachment};

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPass vkrender_pass;
        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_info.pAttachments = attachments.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        // Subpass dependencies
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // TODO: Learn more about this
        VkSubpassDependency depth_dependency{};
        depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depth_dependency.dstSubpass = 0;
        depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.srcAccessMask = 0;
        depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        auto dependencies = std::array{ dependency, depth_dependency };

        render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
        render_pass_info.pDependencies = dependencies.data();
        

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
}