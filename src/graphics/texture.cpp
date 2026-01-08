#include "texture.hpp"

#include "core/log.hpp"
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace kzn {

std::shared_ptr<Texture> Texture::load(const std::filesystem::path& path) {
    int tex_width;
    int tex_height;
    int tex_channels;
    unsigned char* result_ptr = stbi_load(
        path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha
    );

    if (result_ptr == nullptr) {
        throw LoadingError{stbi_failure_reason()};
    }

    return std::make_shared<Texture>(
        result_ptr,
        VkExtent3D{
            .width = static_cast<uint32_t>(tex_width),
            .height = static_cast<uint32_t>(tex_height),
            .depth = 1,
        }
    );
}

} // namespace kzn