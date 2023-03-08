#include "vk/framebuffer.hpp"

#include <cassert>

namespace kzn::vk {
    Attachment::Attachment(AttachmentKind _kind, Device* _device, VkExtent2D _extent)
        : kind(_kind)
        , device(_device)
    {
        // Steps:
        // 1. Create VkImage
        // 2. Create VkImageView
        if(kind == COLOR) {
            // 1. Create VkImage
            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.format = VK_FORMAT_B8G8R8A8_SRGB;
            image_info.extent = VkExtent3D{
                _extent.width,
                _extent.height,
                1
            };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            // Allocate it from GPU local memory
            VmaAllocationCreateInfo img_alloc_info{};
            img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            auto result = vmaCreateImage(device->allocator(), &image_info, &img_alloc_info, &image, &allocation, nullptr);
            VK_CHECK_MSG(result, "Failed to create color image!");

            // 2. Create VkImageView
            VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = image;
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
            result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &image_view);
            VK_CHECK_MSG(result, "Failed to create color image view!");
        }
        else {
            // 1. Create VkImage
            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.format = VK_FORMAT_D32_SFLOAT;
            image_info.extent = VkExtent3D{
                _extent.width,
                _extent.height,
                1
            };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            // Allocate it from GPU local memory
            VmaAllocationCreateInfo depth_img_alloc_info{};
            depth_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            depth_img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            auto result = vmaCreateImage(device->allocator(), &image_info, &depth_img_alloc_info, &image, &allocation, nullptr);
            VK_CHECK_MSG(result, "Failed to create depth image!");

            // 2. Create VkImageView
            VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = image;
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
            result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &image_view);
            VK_CHECK_MSG(result, "Failed to create depth image view!");
        }
    }


    Attachment::Attachment(AttachmentKind _kind, Device* _device, VkImage _image)
        : kind(_kind)
        , image(_image)
        , device(_device)
    {
        if(kind == COLOR) {
            // Create just VkImageView
            VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = image;
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format = VK_FORMAT_B8G8R8A8_SRGB;
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
            auto result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &image_view);
            VK_CHECK_MSG(result, "Failed to create color image view!");
        }
        else {
            // Create VkImageView
            VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = image;
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
            auto result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &image_view);
            VK_CHECK_MSG(result, "Failed to create depth image view!");
        }
    }


    Attachment::Attachment(Attachment&& from)
        : kind(from.kind)
        , device(from.device)
        , image(from.image)
        , image_view(from.image_view)
        , allocation(from.allocation)
    {
        from.moved = true;
    }


    Attachment::~Attachment() {
        if(!moved) {
            if(image_view != VK_NULL_HANDLE) {
                // Destroy image view
                vkDestroyImageView(device->vk_device(), image_view, nullptr);
            }
            if(allocation != VK_NULL_HANDLE) {
                // Destroy image
                vmaDestroyImage(device->allocator(), image, allocation);
            }
        }
    }


    Attachment Attachment::color(Device* device, VkExtent2D extent) {
        return Attachment(COLOR, device, extent);
    }


    Attachment Attachment::depth(Device* device, VkExtent2D extent) {
        return Attachment(DEPTH, device, extent);
    }


    Attachment Attachment::color_from(Device* device, VkImage color_image) {
        return Attachment(COLOR, device, color_image);
    }


    Attachment Attachment::depth_from(Device* device, VkImage depth_image) {
        return Attachment(DEPTH, device, depth_image);
    }


    Framebuffers::Framebuffers(
        Device*                     _device,
        RenderPass*                 _render_pass,
        const std::vector<VkImage>& _color_images,
        VkExtent2D                  _extent)
        : device(_device)
        , render_pass(_render_pass)
        , clear_values{
            VkClearValue{{{0.009, 0.009, 0.009, 1.0f}}},
            VkClearValue{{1.f, 0.f}}
        }
        , extent(_extent)
        , depth_attachmemnt(new Attachment(Attachment::DEPTH, device, _extent))
    {
        // Create attachments and framebuffers
        color_attachmemnts.reserve(_color_images.size());
        framebuffers.reserve(_color_images.size());
        
        // Initialize color attachments
        for(size_t i = 0; i < _color_images.size(); ++i) {
            color_attachmemnts.emplace_back(Attachment::COLOR, device, _color_images[i]);

            // Create framebuffer
            VkFramebuffer framebuffer;
            VkFramebufferCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            create_info.renderPass = render_pass->vk_render_pass();
            create_info.attachmentCount = 2;
            VkImageView attachment_views[] = { color_attachmemnts[i].image_view, depth_attachmemnt->image_view };
            create_info.pAttachments = attachment_views;
            create_info.width = extent.width;
            create_info.height = extent.height;
            create_info.layers = 1;

            auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffer);
            VK_CHECK_MSG(result, "Failed to create framebuffer!");
            framebuffers.emplace_back(framebuffer, extent, clear_values);
        }
        Log::trace("Framebuffers Created");
    }


    Framebuffers::~Framebuffers() {
        // Destroy framebuffers
        for(auto& framebuffer : framebuffers) {
            vkDestroyFramebuffer(device->vk_device(), framebuffer.vk_framebuffer, nullptr);
        }
    }


    void Framebuffers::recreate(
        const std::vector<VkImage>& new_color_images,
        VkExtent2D                  new_extent)
    {
        // Create attachments
        extent = new_extent;
        // NOTE: Big oof
        depth_attachmemnt = std::unique_ptr<Attachment>(new Attachment(Attachment::DEPTH, device, new_extent));
        color_attachmemnts.clear();
        for(size_t i = 0; i < new_color_images.size(); ++i) {
            color_attachmemnts.emplace_back(Attachment::COLOR, device, new_color_images[i]);
        }

        // Recreate framebuffers
        for(size_t i = 0; i < new_color_images.size(); ++i) {
            auto& framebuffer = framebuffers[i];
            // Destroy framebuffer
            vkDestroyFramebuffer(device->vk_device(), framebuffer.vk_framebuffer, nullptr);
            // Create framebuffer
            VkFramebufferCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            create_info.renderPass = render_pass->vk_render_pass();
            create_info.attachmentCount = 2;
            VkImageView attachment_views[] = { color_attachmemnts[i].image_view, depth_attachmemnt->image_view };
            create_info.pAttachments = attachment_views;
            create_info.width = extent.width;
            create_info.height = extent.height;
            create_info.layers = 1;

            auto result = vkCreateFramebuffer(device->vk_device(), &create_info, nullptr, &framebuffer.vk_framebuffer);
            VK_CHECK_MSG(result, "Failed to create framebuffer!");
        }
        Log::trace("Framebuffers recreated");
    }
        
    Framebuffer& Framebuffers::get(uint32_t index) {
        return framebuffers[index];
    }


    void Framebuffers::set_clear_color(const std::array<float,4>& clear_color) noexcept {
        assert(clear_values.size() > 0);
        clear_values[0] = VkClearValue{{{clear_color[0], clear_color[1], clear_color[2], clear_color[3]}}};
    }


    void Framebuffers::set_clear_depth(float clear_depth) noexcept {
        assert(clear_values.size() > 1);
        clear_values[1] = VkClearValue{{clear_depth, 0.f}};
    }

} // namespace kzn::vk