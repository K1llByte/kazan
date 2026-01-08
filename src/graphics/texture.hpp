#pragma once

#include "resources/resource.hpp"
#include "vk/utils.hpp"

#include <filesystem>
#include <vulkan/vulkan_core.h>

namespace kzn {

class Texture {
public:
    // Ctor
    Texture(unsigned char* data, VkExtent3D extent) noexcept
        : m_data{data}
        , m_extent{extent} {}
    // Copy
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // Move
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;
    // Dtor
    ~Texture() = default;

    //! \brief Creates a new texture object with the data from the specified
    //! path.
    //! \param path Relative path to the texture file.
    //! \throws LoadingError if
    [[nodiscard]]
    static std::shared_ptr<Texture> load(const std::filesystem::path& path);

    [[nodiscard]]
    unsigned char const* const data() const noexcept {
        return m_data;
    }

    [[nodiscard]]
    VkExtent3D extent() const noexcept {
        return m_extent;
    }

private:
    unsigned char* m_data;
    VkExtent3D m_extent;
};

} // namespace kzn