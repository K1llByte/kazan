#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <iostream>

namespace kzn
{

    Texture::Texture(Device& device, const std::string& filename)
        : device(device)
    {
        // 1. Create Image
        // NOTE: This texture object is going to only be present in GPU memory
        // 1.1.1 Load texture buffer
        int width, height, channels;
        stbi_uc* pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        // NOTE: 4 channels but texture might have less
        VkDeviceSize image_size = width * height * 4;
        std::cout << "Texture size: (" << width << ", " << height << ", " << channels << ")\n"; 

        if(pixels == nullptr)
        {
            throw std::runtime_error("failed to load texture image!");
        }
        
        // 1.1.2 Create staging buffer
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        device.create_buffer(
            image_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory);

        // 1.1.3 Copy texture data to GPU memory
        void* data;
        vkMapMemory(device.device(), staging_buffer_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(device.device(), staging_buffer_memory);

        // 1.1.4 Free texture data in main memory
        stbi_image_free(pixels);

        // 1.2.1 Create Image for optimal shader acess
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(width);
        image_info.extent.height = static_cast<uint32_t>(height);
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional
        
        if(vkCreateImage(device.device(), &image_info, nullptr, &texture_image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        // 1.2.2 Allocate memory (Not sure about this step)
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(device.device(), texture_image, &mem_requirements);
        
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = device.find_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        if(vkAllocateMemory(device.device(), &alloc_info, nullptr, &texture_image_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }
        
        vkBindImageMemory(device.device(), texture_image, texture_image_memory, 0);

        // 1.3.1 Transition Image layout to DST_OPTIMAL
        transition_image_layout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        // 1.3.2 Copy buffer contents to Image buffer
        copy_buffer_to_image(staging_buffer, texture_image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        // 1.3.3 Transition Image layout to SHADER_READ_ONLY_OPTIMAL
        transition_image_layout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // 1.4. Destroy staging buffer
        vkDestroyBuffer(device.device(), staging_buffer, nullptr);
        vkFreeMemory(device.device(), staging_buffer_memory, nullptr);

        // 2. Create ImageView
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

        if(vkCreateImageView(device.device(), &view_info, nullptr, &texture_image_view) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        // 3. Create Sampler
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        // Filter
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        // Addressing mode
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // Anisotropy filtering
        sampler_info.anisotropyEnable = VK_TRUE;
        // Needs to check the device properties
        const auto limits = device.limits();
        sampler_info.maxAnisotropy = limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        // NOTE: this is for image sampling in [0,1] instead of [0,width:height]
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        if(vkCreateSampler(device.device(), &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void Texture::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
    {
        auto command_buffer = device.begin_single_time_commands();
        
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        // TODO: To avoid this if statements create 2 separate methods for each layout transition
        // 'transition_image_layout_dst_opt'
        // and 'transition_image_layout_shader_read'
        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;
        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            command_buffer,
            source_stage,
            destination_stage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        device.end_single_time_commands(command_buffer);
    }

    void Texture::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer command_buffer = device.begin_single_time_commands();

        // The region to copy will be the whole buffer
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };
        vkCmdCopyBufferToImage(
            command_buffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        device.end_single_time_commands(command_buffer);
    }


    Texture::~Texture()
    {
        vkDestroySampler(device.device(), texture_sampler, nullptr);
        vkDestroyImageView(device.device(), texture_image_view, nullptr);
        vkDestroyImage(device.device(), texture_image, nullptr);
        vkFreeMemory(device.device(), texture_image_memory, nullptr);
    }

}
