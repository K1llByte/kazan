#pragma once

#include "graphics/texture.hpp"
#include "vk/dset.hpp"
#include "vk/image.hpp"

namespace kzn {

struct MaterialData {
    std::optional<TextureData> albedo_opt;
    std::optional<TextureData> normal_opt;
    std::optional<TextureData> metallic_roughness_opt;
    std::optional<TextureData> occlusion_opt;
};

struct Material3D {
    vk::Image albedo_image;
    vk::Image normal_image;
    vk::Image metallic_roughness_image;
    vk::Image occlusion_image;
    vk::DescriptorSet dset;
};

} // namespace kzn