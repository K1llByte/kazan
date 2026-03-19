#pragma once

#include "resources/resource.hpp"
#include "vk/utils.hpp"

#include <filesystem>
#include <vulkan/vulkan_core.h>

namespace kzn {

//! Represents decoded and uncompressed texture data. Can be used to upload
// data directly to the GPU.
struct TextureData {
    unsigned char* bytes;
    Vec3u extent;

    //! Frees allocated image data.
    ~TextureData();

    //! Convert extent member into a VkExtent3D.
    [[nodiscard]]
    constexpr VkExtent3D vk_extent() const;

    //! \brief Creates a new texture data from the specified path.
    //! \param path Relative path to the texture file.
    //! \throws LoadingError
    [[nodiscard]]
    static std::shared_ptr<TextureData> load(const std::filesystem::path& path);
};

} // namespace kzn

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////

namespace kzn {

constexpr VkExtent3D TextureData::vk_extent() const {
    return VkExtent3D{
        .width = extent.x,
        .height = extent.y,
        .depth = extent.z,
    };
}

} // namespace kzn