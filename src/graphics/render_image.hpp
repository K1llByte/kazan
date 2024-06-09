#pragma once

#include "vk/utils.hpp"

namespace kzn {

class RenderImage {
public:
    RenderImage(vk::Device& device, VkExtent3D extent);
    ~RenderImage();

    [[nodiscard]] VkExtent3D extent() const { return m_extent; }
    [[nodiscard]] VkImage image() const { return m_image; }
    [[nodiscard]] VkImageView image_view() const { return m_image_view; }
    [[nodiscard]] VkSampler sampler() const { return m_sampler; }

    void recreate(VkExtent3D extent);

private:
    vk::Device& m_device;
    VkExtent3D m_extent;
    VkImage m_image;
    VmaAllocation m_allocation;
    VkImageView m_image_view;
    VkSampler m_sampler;
};

} // namespace kzn