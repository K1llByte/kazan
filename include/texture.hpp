#ifndef TEXTURE_H
#define TEXTURE_H

#include "device.hpp"

namespace kzn
{

    class Texture
    {
        private:

        Device& device;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        VkImage texture_image;
        VkDeviceMemory texture_image_memory;

        public:

        Texture(Device& device, std::string file);
        ~Texture();
    };

}

#endif // TEXTURE_H
