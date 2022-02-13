#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

namespace kzn
{

    Texture::Texture(Device& device, std::string file)
    {
        // 1.1 Load texture buffer
        int width, height, channels;
        stbi_uc* pixels = stbi_load("textures/texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
        // NOTE: 4 channels but texture might have less
        VkDeviceSize image_size = width * height * 4;

        if(pixels == nullptr)
        {
            throw std::runtime_error("failed to load texture image!");
        }
        
        // 1.2 Create staging buffer
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        device.create_buffer(
            image_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory);

        // 1.3 Copy texture data to GPU memory
        void* data;
        vkMapMemory(device.device(), staging_buffer_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(device.device(), staging_buffer_memory);

        // 1.4 Free texture data in main memory
        stbi_image_free(pixels);

        // 2.1 Create Image for optimal shader acess
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

        // 2.2 Allocate memory (Not sure about this step)
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
        vkCmdPipelineBarrier(
            command_buffer,
            0 /* TODO */,
            0 /* TODO */,
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


    Texture::~Texture()
    {
        vkDestroyImage(device.device(), texture_image, nullptr);
        vkFreeMemory(device.device(), texture_image_memory, nullptr);
    }

}
