#include "vk/image.hpp"

#include "vk/cmd_buffers.hpp"

#include "vk_mem_alloc.h"

namespace kzn::vk {
    Image::Image(Device* _device, VkExtent3D image_extent, bool with_staging_buffer)
        : device(_device)
        , extent(image_extent)
        , has_staging_buffer(with_staging_buffer)
    {
        // Steps:
        // 1. Create staging buffer
        // 2. Create image
        // 3. Create image view
        // 4. Create image sampler
        
        // Image format: VK_FORMAT_R8G8B8A8_SRGB
        // Pixel size for format: 4 bytes
        const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        const uint32_t pixel_size = 4;
        image_size =  
            image_extent.width *
            image_extent.height * 
            image_extent.depth * 
            pixel_size;
        
        // 1. Create staging buffer
        if(has_staging_buffer) {
            VkBufferCreateInfo buffer_info = {};
            buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            // This is the total size, in bytes, of the buffer we are allocating
            buffer_info.size = image_size;
            // This buffer is going to be used as a Vertex Buffer
            buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            // Let the VMA library know that this data should be writeable by CPU
            VmaAllocationCreateInfo buffer_alloc_info = {};
            buffer_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

            // Allocate the staging buffer
            auto result = vmaCreateBuffer(
                device->allocator(),
                &buffer_info,
                &buffer_alloc_info,
                &staging_buffer,
                &staging_buffer_allocation,
                nullptr);
            VK_CHECK_MSG(result, "Failed to allocate image staging buffer");
        }

        // 2. Create image
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent = image_extent;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        // Image format
        image_info.format = format;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.flags = 0; // Optional

        VmaAllocationCreateInfo image_alloc_info = {};
        image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        auto result = vmaCreateImage(
            device->allocator(),
            &image_info,
            &image_alloc_info,
            &texture_image,
            &texture_image_allocation,
            nullptr);
        VK_CHECK_MSG(result, "Failed to allocate texture image");

        // 3. Create image view
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = texture_image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device->vk_device(), &view_info, nullptr, &texture_image_view);
        VK_CHECK_MSG(result, "Failed to create texture image view");

        // 4. Create image sampler
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        // Get PhysicalDeviceProperties
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device->vk_physical_device(), &properties);
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        result = vkCreateSampler(device->vk_device(), &sampler_info, nullptr, &texture_sampler);
        VK_CHECK_MSG(result, "Failed to create texture sampler");
    }


    Image::~Image() {
        // Destroy texture sampler
        vkDestroySampler(device->vk_device(), texture_sampler, nullptr);
        // Destroy texture image view
        vkDestroyImageView(device->vk_device(), texture_image_view, nullptr);
        // Destroy texture image
        vmaDestroyImage(device->allocator(), texture_image, texture_image_allocation);
        if(has_staging_buffer) {
            // Destroy staging buffer
            vmaDestroyBuffer(device->allocator(), staging_buffer, staging_buffer_allocation);
        }
    }


    VkDescriptorImageInfo Image::info() const noexcept {
        return VkDescriptorImageInfo{
            .sampler = texture_sampler,
            .imageView = texture_image_view,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
    }


    void Image::init_staging_buffer() {
        // 1. Create staging buffer
        if(has_staging_buffer) return;
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        // This is the total size, in bytes, of the buffer we are allocating
        buffer_info.size = image_size;
        // This buffer is going to be used as a Vertex Buffer
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        // Let the VMA library know that this data should be writeable by CPU
        VmaAllocationCreateInfo buffer_alloc_info = {};
        buffer_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

        // Allocate the staging buffer
        auto result = vmaCreateBuffer(
            device->allocator(),
            &buffer_info,
            &buffer_alloc_info,
            &staging_buffer,
            &staging_buffer_allocation,
            nullptr);
        VK_CHECK_MSG(result, "Failed to allocate image staging buffer");
        has_staging_buffer = true;
    }


    void Image::upload(void* data) {
        // Steps:
        // 1. Upload data to staging buffer
        // 2. Copy data to image
        // 2.1. Transition image to transfer dst layout
        // 2.2. Copy buffer to image
        // 2.3. Transition image to shader read optimal layout

        if(has_staging_buffer) {
            // 1. Upload data to staging buffer
            void* mapped_memory;
            vmaMapMemory(device->allocator(), staging_buffer_allocation, &mapped_memory);
            memcpy(mapped_memory, data, static_cast<size_t>(image_size));
            vmaUnmapMemory(device->allocator(), staging_buffer_allocation);

            // 2. Copy data to image
            device->immediate_submit([&](vk::CommandBuffer& cmd_buffer){
                // 2.1. Transition image to transfer dst layout
                VkImageSubresourceRange range;
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;

                VkImageMemoryBarrier image_barrier_transfer_dst = {};
                image_barrier_transfer_dst.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                image_barrier_transfer_dst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                image_barrier_transfer_dst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                image_barrier_transfer_dst.image = texture_image;
                image_barrier_transfer_dst.subresourceRange = range;
                image_barrier_transfer_dst.srcAccessMask = 0;
                image_barrier_transfer_dst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                // Barrier the image into the transfer-receive layout
                vkCmdPipelineBarrier(
                    cmd_buffer.vk_command_buffer(),
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &image_barrier_transfer_dst);

                // 2.2. Copy buffer to image
                VkBufferImageCopy copy_region = {};
                copy_region.bufferOffset = 0;
                copy_region.bufferRowLength = 0;
                copy_region.bufferImageHeight = 0;
                copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copy_region.imageSubresource.mipLevel = 0;
                copy_region.imageSubresource.baseArrayLayer = 0;
                copy_region.imageSubresource.layerCount = 1;
                copy_region.imageExtent = extent;

                // Copy the staging buffer into the image
                vkCmdCopyBufferToImage(cmd_buffer.vk_command_buffer(), staging_buffer, texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

                // 2.3. Transition image to shader read optimal layout
                VkImageMemoryBarrier image_barrier_shader_readeable = image_barrier_transfer_dst;
                image_barrier_shader_readeable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                image_barrier_shader_readeable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                image_barrier_shader_readeable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                image_barrier_shader_readeable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                // Barrier the image into the shader readable layout
                vkCmdPipelineBarrier(cmd_buffer.vk_command_buffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_barrier_shader_readeable);
            });
        }
        else {
            throw NoStagingBuffer();
        }

    }
} // namespace kzn::vk