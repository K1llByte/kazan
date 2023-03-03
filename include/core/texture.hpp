#pragma once

#include "vk/utils.hpp"

#include <filesystem>

namespace kzn {
    class Texture {
        public:
        static Texture load(const char* file_path);

        VkExtent3D get_extent() const noexcept {
            return extent;
        }
        unsigned char* const get_data() const noexcept {
            return data;
        }
        
        private:
        Texture(unsigned char*, VkExtent3D);
        
        private:
        unsigned char* data;
        VkExtent3D     extent;
    };
} // namespace kzn