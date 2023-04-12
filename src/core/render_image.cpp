#include "core/render_image.hpp"

#include "vk/utils.hpp"

namespace kzn
{
    RenderImage::RenderImage(vk::Device* device, VkExtent3D extent)
        : m_device(device)
        , m_extent(extent)
    {
        Log::debug("Created RenderImage");

        // 1. Create VkImage
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.format = VK_FORMAT_B8G8R8A8_SRGB;
        image_info.extent = extent;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo img_alloc_info{};
        img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto result = vmaCreateImage(
            m_device->allocator(),
            &image_info,
            &img_alloc_info,
            &m_image,
            &m_allocation,
            nullptr
        );
        VK_CHECK_MSG(result, "Failed to create color image!");

        // 3. Create VkImageView
        m_image_view = vk::create_image_view(
            *m_device,
            m_image,
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT
        );

        // 4. Create VkSampler
        m_sampler = vk::create_sampler(*m_device);
    }


    RenderImage::~RenderImage()
    {
        Log::debug("Destroyed RenderImage");
        // 1. Destroy VkImage
        vmaDestroyImage(
            m_device->allocator(),
            m_image, 
            m_allocation
        );

        // 2. Destroy VkImageView
        vk::destroy_image_view(
            *m_device,
            m_image_view
        );

        // 3. Destroy VkSampler
        vk::destroy_sampler(*m_device, m_sampler);
    }


    void RenderImage::recreate(VkExtent3D extent)
    {
        m_extent = extent;

        // 1. Recreate VkImage
        // 1.1. Destroy VkImage
        vmaDestroyImage(
            m_device->allocator(),
            m_image, 
            m_allocation
        );
        // m_image = VK_NULL_HANDLE;
        // 1.2. Create VkImage
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.format = VK_FORMAT_B8G8R8A8_SRGB;
        image_info.extent = extent;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo img_alloc_info{};
        img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        auto result = vmaCreateImage(
            m_device->allocator(),
            &image_info,
            &img_alloc_info,
            &m_image,
            &m_allocation,
            nullptr
        );
        VK_CHECK_MSG(result, "Failed to create color image!");


        // 2. Recreate VkImageView
        // 2.1. Destroy VkImageView
        vk::destroy_image_view(
            *m_device,
            m_image_view
        );

        // 2.1. Create VkImageView
        m_image_view = vk::create_image_view(
            *m_device,
            m_image,
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
} // namespace kzn
