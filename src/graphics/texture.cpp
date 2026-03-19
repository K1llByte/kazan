#include "texture.hpp"

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace kzn {

TextureData::~TextureData() {
    stbi_image_free(bytes);
}

std::shared_ptr<TextureData> TextureData::load(const std::filesystem::path& path) {
    int width;
    int height;
    int channels;
    unsigned char* result_ptr = stbi_load(
        path.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );
    if (result_ptr == nullptr) {
        throw LoadingError{stbi_failure_reason()};
    }

    return std::make_shared<TextureData>(
        result_ptr,
        Vec3u{static_cast<uint>(width), static_cast<uint>(height), 1}
    );
}

} // namespace kzn