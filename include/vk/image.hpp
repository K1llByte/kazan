#ifndef KZN_VK_IMAGE_HPP
#define KZN_VK_IMAGE_HPP

#include "vk/device.hpp"

namespace kzn::vk {
    class Image {
        public:
        Image(Device* _device, VkExtent3D image_extent);
        ~Image();

        void upload(void* data);

        private:
        // Image data
        uint64_t image_size;
        // Vulkan data
        Device*        device;
        VkBuffer       staging_buffer;
        VmaAllocation  staging_buffer_allocation;
        VkImage        texture_image;
        VmaAllocation  texture_image_allocation;
    };
} // namespace kzn::vk

#endif // KZN_VK_IMAGE_HPP