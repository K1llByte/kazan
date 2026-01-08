#pragma once

#include "vk/device.hpp"
#include "vk/dset.hpp"
#include <vulkan/vulkan_core.h>
namespace kzn::vk {

class Image {
public:
    // Ctor
    Image(
        Device& device,
        VkExtent3D extent,
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
        VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT,
        VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT
    );
    // Copy
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    // Move
    Image(Image&&);
    Image& operator=(Image&&);
    // Dtor
    ~Image();

    [[nodiscard]]
    uint64_t size() const noexcept;

    [[nodiscard]]
    VkImageView vk_image_view() const noexcept {
        return m_texture_image_view;
    }

    [[nodiscard]]
    VkImage vk_image() const noexcept {
        return m_texture_image;
    }

    [[nodiscard]]
    DescriptorInfo info() const noexcept;

    void upload(const void* data);

private:
    Device* m_device_ptr;
    VkExtent3D m_extent;
    // Staging buffer
    VkBuffer m_staging_buffer;
    VmaAllocation m_staging_buffer_allocation;
    // Texture image
    VkImage m_texture_image;
    VmaAllocation m_texture_image_allocation;
    VkImageView m_texture_image_view;
    VkSampler m_texture_sampler;

private:
    void delete_image_data();
};

} // namespace kzn::vk