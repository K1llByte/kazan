#pragma once

#include "vk/device.hpp"

namespace kzn
{
    class RenderImage
    {
        public:
        RenderImage(vk::Device* device, VkExtent3D extent);
        ~RenderImage();

        VkExtent3D  extent() const noexcept { return m_extent; }
        VkImage     image() const noexcept { return m_image; }
        VkImageView image_view() const noexcept { return m_image_view; }
        VkSampler   sampler() const noexcept { return m_sampler; }


        void recreate(VkExtent3D extent);

        private:
        vk::Device*   m_device;
        VkExtent3D    m_extent;
        VkImage       m_image;
        VmaAllocation m_allocation;
        VkImageView   m_image_view;
        VkSampler     m_sampler;
        
    };
} // namespace kzn