#include "vk/framebuffer.hpp"

namespace kzn::vk {
    Attachment Attachment::color(Device* device, VkExtent3D extent) {
        // Steps:
        // 1. Create VkImage
        // 2. Create VkImageView

        VkImage       color_image;
        VkImageView   color_image_view;
        VmaAllocation color_allocation;

        // 1. Create VkImage
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.extent = extent;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        // Allocate it from GPU local memory
        VmaAllocationCreateInfo img_alloc_info{};
        img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto result = vmaCreateImage(device->allocator(), &image_info, &img_alloc_info, &color_image, &color_allocation, nullptr);
        VK_CHECK_MSG(result, "Failed to create color image!");

        // 2. Create VkImageView
        VkImageViewCreateInfo image_view_info{};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = color_image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
        result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &color_image_view);
        VK_CHECK_MSG(result, "Failed to create color image view!");
        
        return Attachment {
            .kind = COLOR,
            .device = device,
            .image = color_image,
            .image_view = color_image_view,
            .allocation = color_allocation,
        };
    }
    

    Attachment Attachment::depth(Device* device, VkExtent3D extent) {
        // Steps:
        // 1. Create VkImage
        // 2. Create VkImageView

        VkImage       depth_image;
        VkImageView   depth_image_view;
        VmaAllocation depth_allocation;

        // 1. Create VkImage
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.format = VK_FORMAT_D32_SFLOAT;
        image_info.extent = extent;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        // Allocate it from GPU local memory
        VmaAllocationCreateInfo depth_img_alloc_info{};
        depth_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        depth_img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto result = vmaCreateImage(device->allocator(), &image_info, &depth_img_alloc_info, &depth_image, &depth_allocation, nullptr);
        VK_CHECK_MSG(result, "Failed to create depth image!");

        // 2. Create VkImageView
        VkImageViewCreateInfo image_view_info{};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = depth_image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = VK_FORMAT_D32_SFLOAT;
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
        result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &depth_image_view);
        VK_CHECK_MSG(result, "Failed to create depth image view!");
        
        return Attachment {
            .kind = DEPTH,
            .device = device,
            .image = depth_image,
            .image_view = depth_image_view,
            .allocation = depth_allocation,
        };
    }


    Attachment Attachment::color_from(Device* device, VkImage color_image) {
        VkImageView color_image_view;

        // Create VkImageView
        VkImageViewCreateInfo image_view_info{};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = color_image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
        auto result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &color_image_view);
        VK_CHECK_MSG(result, "Failed to create color image view!");

        return Attachment {
            .kind = COLOR,
            .device = device,
            .image = color_image,
            .image_view = color_image_view,
            .allocation = VK_NULL_HANDLE,
        };
    }


    Attachment Attachment::depth_from(Device* device, VkImage depth_image) {
        VkImageView depth_image_view;

        // Create VkImageView
        VkImageViewCreateInfo image_view_info{};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = depth_image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = VK_FORMAT_D32_SFLOAT;
        image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
        auto result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &depth_image_view);
        VK_CHECK_MSG(result, "Failed to create depth image view!");

        return Attachment {
            .kind = DEPTH,
            .device = device,
            .image = depth_image,
            .image_view = depth_image_view,
            .allocation = VK_NULL_HANDLE,
        };
    }


    Attachment::~Attachment() {
        // Destroy image view
        vkDestroyImageView(device->vk_device(), image_view, nullptr);
        if(allocation != VK_NULL_HANDLE) {
            // Destroy image
            vmaDestroyImage(device->allocator(), image, allocation);
        }
    }


    Framebuffer::Framebuffer(
        Device*                                  _device,
        RenderPass&                              _render_pass,
        const std::initializer_list<Attachment>& _attachments,
        VkExtent3D                               _extent)
        : device(device)
        , vk_render_pass(_render_pass.vk_render_pass())
    {
        std::vector<VkImageView> attachments;
        attachments.reserve(_attachments.size());
        auto it = _attachments.begin();
        for(size_t i = 0; i < _attachments.size(); ++i) {
            attachments.push_back(it->image_view);
            ++it;
        }
        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = vk_render_pass;
        create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        create_info.pAttachments = attachments.data();
        create_info.width = _extent.width;
        create_info.height = _extent.height;
        create_info.layers = 1;

        auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffer);
        VK_CHECK_MSG(result, "Failed to create framebuffer!");
    }


    void Framebuffer::recreate(
        const std::initializer_list<Attachment>& _attachments,
        VkExtent3D new_extent)
    {
        std::vector<VkImageView> attachments;
        attachments.reserve(_attachments.size());
        auto it = _attachments.begin();
        for(size_t i = 0; i < _attachments.size(); ++i) {
            attachments.push_back(it->image_view);
            ++it;
        }
        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = vk_render_pass;
        create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        create_info.pAttachments = attachments.data();
        create_info.width = new_extent.width;
        create_info.height = new_extent.height;
        create_info.layers = 1;

        auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffer);
        VK_CHECK_MSG(result, "Failed to create framebuffer!");
    }


    Framebuffer::~Framebuffer() {
        vkDestroyFramebuffer(device->vk_device(), framebuffer, nullptr);
    }

} // namespace kzn::vk