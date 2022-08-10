#include "vk/image.hpp"

#include "vk_mem_alloc.h"

namespace kzn::vk {
    Image::Image(Device* _device, VkExtent3D image_extent)
        : device(_device)
        , image_size( 
            image_extent.width *
            image_extent.height * 
            image_extent.depth * 
            pixel_size)
    {
        // Steps:
        // 1. Create staging buffer
        // 2. Create image
        
        // Image format: VK_FORMAT_R8G8B8A8_SRGB
        // Pixel size for format: 4 bytes
        const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        const uint32_t pixel_size = 4;
        
        
        // Allocate vertex buffer
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
        VK_CHECK_MSG(vmaCreateBuffer(
            device->allocator(),
            &buffer_info,
            &buffer_alloc_info,
            &staging_buffer,
            &staging_buffer_allocation,
            nullptr), "Failed to allocate image staging buffer");

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

        VK_CHECK_MSG(vmaCreateImage(
            device->allocator(),
            &image_info,
            &image_alloc_info,
            &texture_image,
            &texture_image_allocation,
            nullptr), "Failed to allocate texture image");
    }


    Image::~Image() {
        // Destroy texture image
        vmaDestroyImage(device->allocator(), texture_image, texture_image_allocation);
        // Destroy staging buffer
        vmaDestroyBuffer(device->allocator(), staging_buffer, staging_buffer_allocation);
    }


    void upload(void* data) {
        // Steps:
        // 1. Upload data to staging buffer
        // 2. Copy data to image
        // 2.1. Transition image
        // 2.2. Copy buffer to image

        // 1. Upload data to staging buffer
        void* mapped_memory;
        vmaMapMemory(device->allocator(), staging_buffer_allocation, &mapped_memory);
        memcpy(mapped_memory, data, static_cast<size_t>(image_size));
        vmaUnmapMemory(device->allocator(), staging_buffer_allocation);

        // 2. Copy data to image
        // 2.1. Transition image

        // 2.2. Copy buffer to image
        
    }
} // namespace kzn::vk