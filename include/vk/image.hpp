#pragma once

#include "vk/device.hpp"
#include "vk/descriptor_set.hpp"

namespace kzn::vk {
    class Image {
        public:
        Image(Device* _device, VkExtent3D image_extent, bool _has_staging_buffer = true);
        ~Image();

        VkImage     get_image() noexcept { return texture_image; }
        VkImageView get_image_view() noexcept { return texture_image_view; }
        VkSampler   get_sampler() noexcept { return texture_sampler; }

        DescriptorInfo info() const noexcept;

        void init_staging_buffer();
        void upload(void* data);

        private:
        // Image data
        uint64_t       image_size;
        VkExtent3D     extent;
        // Vulkan data
        bool has_staging_buffer;
        Device*        device;
        VkBuffer       staging_buffer;
        VmaAllocation  staging_buffer_allocation;
        VkImage        texture_image;
        VmaAllocation  texture_image_allocation;
        VkImageView    texture_image_view;
        VkSampler      texture_sampler;
    };
} // namespace kzn::vk