#pragma once

#include "vk/dset.hpp"
#include "vk/image.hpp"
namespace kzn {

struct Material3D {
    vk::Image albedo_image;
    vk::DescriptorSet dset;
};

} // namespace kzn