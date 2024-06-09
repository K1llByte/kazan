#include "render_image.hpp"

#include "core/log.hpp"

namespace kzn {

///////////////// TEMPORARY /////////////////
namespace vk {

VkImageView create_image_view(
    vk::Device& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_mask
) {
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = aspect_mask;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkImageView image_view;
    auto result =
        vkCreateImageView(device.vk_device(), &view_info, nullptr, &image_view);
    VK_CHECK_MSG(result, "Failed to create image view");
    return image_view;
}

VkSampler create_sampler(vk::Device& device) {
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_FALSE;
    // Get PhysicalDeviceProperties
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device.vk_physical_device(), &properties);
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    VkSampler sampler;
    auto result =
        vkCreateSampler(device.vk_device(), &sampler_info, nullptr, &sampler);
    VK_CHECK_MSG(result, "Failed to create sampler");
    return sampler;
}

void destroy_image_view(vk::Device& device, VkImageView image_view) {
    vkDestroyImageView(device.vk_device(), image_view, nullptr);
}

void destroy_sampler(vk::Device& device, VkSampler sampler) {
    vkDestroySampler(device.vk_device(), sampler, nullptr);
}

} // namespace vk

/////////////////////////////////////////////

RenderImage::RenderImage(vk::Device& device, VkExtent3D extent)
    : m_device(device)
    , m_extent(extent) {
    Log::trace("Created RenderImage");

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
    image_info.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

    VmaAllocationCreateInfo img_alloc_info{};
    img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    img_alloc_info.requiredFlags =
        VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    auto result = vmaCreateImage(
        m_device.allocator(),
        &image_info,
        &img_alloc_info,
        &m_image,
        &m_allocation,
        nullptr
    );
    VK_CHECK_MSG(result, "Failed to create color image!");

    // 3. Create VkImageView
    m_image_view = vk::create_image_view(
        m_device, m_image, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT
    );

    // 4. Create VkSampler
    m_sampler = vk::create_sampler(m_device);
}

RenderImage::~RenderImage() {
    Log::trace("Destroyed RenderImage");
    // 1. Destroy VkImage
    vmaDestroyImage(m_device.allocator(), m_image, m_allocation);

    // 2. Destroy VkImageView
    vk::destroy_image_view(m_device, m_image_view);

    // 3. Destroy VkSampler
    vk::destroy_sampler(m_device, m_sampler);
}

} // namespace kzn