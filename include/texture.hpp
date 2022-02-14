#ifndef TEXTURE_H
#define TEXTURE_H

#include "device.hpp"

namespace kzn
{

    class Texture
    {
    private:
        Device &device;
        VkImage texture_image;
        VkImageView texture_image_view;
        VkDeviceMemory texture_image_memory;

    public:
        Texture(Device& device, const std::string& filename);
        ~Texture();

    private:
        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
        void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    };

}

#endif // TEXTURE_H
