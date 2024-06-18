#pragma once

#include "vk/utils.hpp"

namespace kzn {

class RenderImage {
public:
    // Ctor
    RenderImage(
        vk::Device& device,
        VkExtent2D extent,
        VkFormat format = VK_FORMAT_B8G8R8A8_SRGB
    );
    // Copy
    RenderImage(const RenderImage&) = delete;
    RenderImage& operator=(const RenderImage&) = delete;
    // Move
    RenderImage(RenderImage&&) = delete;
    RenderImage& operator=(RenderImage&&) = delete;
    // Dtor
    ~RenderImage();

    [[nodiscard]]
    VkExtent2D extent() const {
        return m_extent;
    }
    [[nodiscard]]
    VkImage image() const {
        return m_image;
    }
    [[nodiscard]]
    VkImageView image_view() const {
        return m_image_view;
    }
    [[nodiscard]]
    VkSampler sampler() const {
        return m_sampler;
    }

    void recreate(VkExtent3D extent);

private:
    vk::Device& m_device;
    VkExtent2D m_extent;
    VkImage m_image;
    VkFormat m_format;
    VmaAllocation m_allocation;
    VkImageView m_image_view;
    VkSampler m_sampler;
};

} // namespace kzn