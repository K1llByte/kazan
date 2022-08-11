#ifndef KZN_VK_FRAMEBUFFER_HPP
#define KZN_VK_FRAMEBUFFER_HPP

#include "vk/render_pass.hpp"

namespace kzn::vk {
    struct Attachment {
        enum AttachmentKind: uint8_t {
            COLOR,
            DEPTH,
        };

        AttachmentKind kind;
        Device*        device;
        VkImage        image;
        VkImageView    image_view;
        VmaAllocation  allocation;

        public:
        ~Attachment();
        
        static Attachment color(Device* device, VkExtent3D extent);
        static Attachment depth(Device* device, VkExtent3D extent);

        // Attachment wont destroy provided VkImage since it was provided from outside
        static Attachment color_from(Device* device, VkImage color_image);
        static Attachment depth_from(Device* device, VkImage depth_image);

    };

    class Framebuffer {
        public:
        Framebuffer(
            Device*                                  device,
            RenderPass&                              render_pass,
            const std::initializer_list<Attachment>& attachments,
            VkExtent3D                               extent);
        ~Framebuffer();

        void recreate(
            const std::initializer_list<Attachment>& _attachments,
            VkExtent3D new_extent);

        private:
        Device*       device;
        VkRenderPass  vk_render_pass;
        VkFramebuffer framebuffer;
    };
} // namespace kzn::vk

#endif // KZN_VK_FRAMEBUFFER_HPP