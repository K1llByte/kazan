#ifndef TYPES_H
#define TYPES_H

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <vulkan/vulkan.h>

// #define VMA_USE_STL_CONTAINERS 1

struct AllocatedBuffer
{
    VkBuffer      _buffer;
    // VmaAllocation _allocation;
};

namespace kzn
{


}

#endif // TYPES_H
