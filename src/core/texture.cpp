#include "core/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace kzn {
    Texture Texture::load(const char* file_path) {
        int tex_width, tex_height, tex_channels;
        auto* loaded_data = stbi_load(
            file_path,
            &tex_width,
            &tex_height,
            &tex_channels,
            STBI_rgb_alpha);

        if(loaded_data == nullptr) {
            throw std::runtime_error("Failed to load texture image");
        }
        
        return Texture(
            loaded_data,
            VkExtent3D{
                .width = static_cast<uint32_t>(tex_width),
                .height = static_cast<uint32_t>(tex_height),
                .depth = 1,
            }
        );
    }


    Texture::Texture(unsigned char* _data, VkExtent3D _extent)
        : data(_data), extent(_extent) {}
} // namespace kzn
