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
        VkDeviceMemory texture_image_memory;

    public:
        Texture(Device &device, std::string file);
        ~Texture();

        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
    };

}

#endif // TEXTURE_H
