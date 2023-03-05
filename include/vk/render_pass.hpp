#ifndef KZN_VK_RENDER_PASS_HPP
#define KZN_VK_RENDER_PASS_HPP

#include "vk/device.hpp"
#include "vk/swapchain.hpp"
#include "vk/cmd_buffers.hpp"

namespace kzn::vk
{
    struct AttachmentDesc {
        VkAttachmentDescription data;

        constexpr AttachmentDesc(VkFormat format);
        constexpr AttachmentDesc& set_flags(VkAttachmentDescriptionFlags);
        constexpr AttachmentDesc& set_samples(VkSampleCountFlagBits);
        constexpr AttachmentDesc& set_load_op(VkAttachmentLoadOp);
        constexpr AttachmentDesc& set_store_op(VkAttachmentStoreOp);
        constexpr AttachmentDesc& set_stencil_load_op(VkAttachmentLoadOp);
        constexpr AttachmentDesc& set_stencil_store_op(VkAttachmentStoreOp);
        constexpr AttachmentDesc& set_initial_layout(VkImageLayout);
        constexpr AttachmentDesc& set_final_layout(VkImageLayout);
    };


    struct SubpassDesc {
        VkSubpassDescriptionFlags flags = 0;
        VkPipelineBindPoint       bind_point;
        std::vector<size_t>       input_attachments;
        std::vector<size_t>       color_attachments;
        std::vector<size_t>       resolve_attachments; // Same size as color_attachmemnts
        // If its "-1" then there's no depth buffer
        size_t                    depth_stencil_attachment = size_t(-1);
        std::vector<uint32_t>     preserve_attachments;

        constexpr SubpassDesc(VkPipelineBindPoint _bind_point);
        constexpr SubpassDesc& set_flags(VkSubpassDescriptionFlags);
        constexpr SubpassDesc& set_input_attachments(const std::initializer_list<size_t>&);
        constexpr SubpassDesc& set_color_attachments(const std::initializer_list<size_t>&);
        constexpr SubpassDesc& set_resolve_attachments(const std::initializer_list<size_t>&);
        constexpr SubpassDesc& set_depth_stencil_attachment(size_t);
        constexpr SubpassDesc& set_preserve_attachments(const std::initializer_list<uint32_t>&);
    };


    struct Framebuffer {
        VkFramebuffer              vk_framebuffer;
        VkExtent2D&                extent;
        std::vector<VkClearValue>& clear_values;
    };


    class RenderPass
    {
        friend class RenderPassBuilder;

        public:
        ~RenderPass();
    
        VkRenderPass vk_render_pass() noexcept { return vkrender_pass; }

        void begin(CommandBuffer& cmd_buffer, Framebuffer& framebuffer);
        void end(CommandBuffer& cmd_buffer);

        private:
        RenderPass(Device* _device, VkRenderPass _render_pass);

        private:
        VkRenderPass vkrender_pass;
        Device*      device;
    };


    class RenderPassBuilder
    {
        public:
        constexpr RenderPassBuilder(Device* device);

        constexpr RenderPassBuilder& add_attachment(const vk::AttachmentDesc&, VkImageLayout ref_layout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;
        constexpr RenderPassBuilder& add_subpass(const vk::SubpassDesc&) noexcept;
        constexpr RenderPassBuilder& add_subpass_dependency(const VkSubpassDependency&) noexcept;

        RenderPass build();

        private:
        Device*                            device;
        VkFormat                           color_format;
        size_t                             attachment_index = 0;

        std::vector<VkAttachmentDescription> attachments_descriptions;
        std::vector<VkAttachmentReference>   attachment_refs;
        std::vector<vk::SubpassDesc>         subpasses_descriptions;
        std::vector<VkSubpassDependency>     subpasses_dependencies;
    };
}

////////////////////// Implementation //////////////////////

namespace kzn::vk {
    // Attachement Description
    constexpr AttachmentDesc::AttachmentDesc(VkFormat format)
        : data{
            .format = format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        } {}

    constexpr AttachmentDesc& AttachmentDesc::set_flags(VkAttachmentDescriptionFlags flags) {
        data.flags = flags;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_samples(VkSampleCountFlagBits samples) {
        data.samples = samples;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_load_op(VkAttachmentLoadOp load_op) {
        data.loadOp = load_op;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_store_op(VkAttachmentStoreOp store_op) {
        data.storeOp = store_op;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_stencil_load_op(VkAttachmentLoadOp stencil_load_op) {
        data.stencilLoadOp = stencil_load_op;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_stencil_store_op(VkAttachmentStoreOp stencil_store_op) {
        data.stencilStoreOp = stencil_store_op;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_initial_layout(VkImageLayout initial_layout) {
        data.initialLayout = initial_layout;
        return *this;
    }

    constexpr AttachmentDesc& AttachmentDesc::set_final_layout(VkImageLayout final_layout) {
        data.finalLayout = final_layout;
        return *this;
    }

    // Subpass Description
    constexpr SubpassDesc::SubpassDesc(VkPipelineBindPoint _bind_point) {
        bind_point = _bind_point;
    }

    constexpr SubpassDesc& SubpassDesc::set_flags(VkSubpassDescriptionFlags _flags) {
        flags = _flags;
        return *this;
    }

    constexpr SubpassDesc& SubpassDesc::set_input_attachments(const std::initializer_list<size_t>& _input_attachments) {
        input_attachments = _input_attachments;
        return *this;
    }

    constexpr SubpassDesc& SubpassDesc::set_color_attachments(const std::initializer_list<size_t>& _color_attachments) {
        color_attachments = _color_attachments;
        return *this;
    }

    constexpr SubpassDesc& SubpassDesc::set_resolve_attachments(const std::initializer_list<size_t>& _resolve_attachments) {
        resolve_attachments = _resolve_attachments;
        return *this;
    }

    constexpr SubpassDesc& SubpassDesc::set_depth_stencil_attachment(size_t _depth_stencil_attachment) {
        depth_stencil_attachment = _depth_stencil_attachment;
        return *this;
    }

    constexpr SubpassDesc& SubpassDesc::set_preserve_attachments(const std::initializer_list<uint32_t>& _preserve_attachments) {
        preserve_attachments = _preserve_attachments;
        return *this;
    }
    
    // RenderPassBuilder
    constexpr RenderPassBuilder::RenderPassBuilder(Device* device)
        : device(device)
    {
        attachments_descriptions.reserve(3);
        attachment_refs.reserve(3);
        subpasses_descriptions.reserve(3);
        subpasses_dependencies.reserve(3);
    }
    
    constexpr RenderPassBuilder& RenderPassBuilder::add_attachment(const vk::AttachmentDesc& attachment_description, VkImageLayout ref_layout) noexcept {
        attachments_descriptions.push_back(attachment_description.data);
        // VUID-VkAttachmentReference-layout-03077
        // If an attachment is unused, it doesn't need to have a reference
        // if(ref_layout != VK_IMAGE_LAYOUT_UNDEFINED) {
        attachment_refs.emplace_back(
            static_cast<uint32_t>(attachment_index),
            ref_layout
        );
        // }
        ++attachment_index;
        return *this;
    }


    constexpr RenderPassBuilder& RenderPassBuilder::add_subpass(const vk::SubpassDesc& subpass_description) noexcept {
        subpasses_descriptions.push_back(subpass_description);
        return *this;
    }

    constexpr RenderPassBuilder& RenderPassBuilder::add_subpass_dependency(const VkSubpassDependency& subpass_dependency) noexcept {
        subpasses_dependencies.push_back(subpass_dependency);
        return *this;
    }   
}

#endif // KZN_VK_RENDER_PASS_HPP