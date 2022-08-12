#ifndef KZN_VK_FRAMEBUFFER_HPP
#define KZN_VK_FRAMEBUFFER_HPP

#include "vk/render_pass.hpp"

#include <memory>

namespace kzn::vk {
    struct Attachment {

        enum AttachmentKind: uint8_t {
            COLOR,
            DEPTH,
        } kind;
        Device*        device;
        VkImage        image;
        VkImageView    image_view = VK_NULL_HANDLE;
        VmaAllocation  allocation = VK_NULL_HANDLE;
        bool           moved = false;

        public:
        // Create 
        Attachment(AttachmentKind kind, Device* device, VkExtent2D extent);
        // Create from image
        // Attachment wont destroy provided VkImage since it was provided from outside
        Attachment(AttachmentKind kind, Device* device, VkImage image);
        Attachment(Attachment&&);
        // Attachment(Attachment&&) = default;
        ~Attachment();
        Attachment& operator=(Attachment&& other) {
            other.moved = true;
            this->kind = other.kind;
            this->device = other.device;
            this->image = other.image;
            this->image_view = other.image_view;
            this->allocation = other.allocation;
            return *this;
        }

        // TODO: Delete copy & move ctor
        
        // Create 
        static Attachment color(Device* device, VkExtent2D extent);
        static Attachment depth(Device* device, VkExtent2D extent);

        // Create from image
        // Attachment wont destroy provided VkImage since it was provided from outside
        static Attachment color_from(Device* device, VkImage color_image);
        static Attachment depth_from(Device* device, VkImage depth_image);
    };

    // Framebuffer declared in "vk/render_pass.hpp"

    class Framebuffers {
        public:
        Framebuffers(
            Device*,
            RenderPass*,
            const std::vector<VkImage>&,
            VkExtent2D);
        ~Framebuffers();
        // Framebuffers(Device*, RenderPass*, VkExtent2D)

        void recreate(
            const std::vector<VkImage>&,
            VkExtent2D);
        
        Framebuffer& get(uint32_t index);

        void set_clear_color(const std::array<float,4>&) noexcept;
        void set_clear_depth(float) noexcept;

        private:
        Device*                         device;
        RenderPass*                     render_pass;
        std::vector<vk::Attachment>     color_attachmemnts;
        std::unique_ptr<vk::Attachment> depth_attachmemnt;
        std::vector<Framebuffer>        framebuffers;
        std::vector<VkClearValue>       clear_values;
        VkExtent2D                      extent;
    };
} // namespace kzn::vk

#endif // KZN_VK_FRAMEBUFFER_HPP