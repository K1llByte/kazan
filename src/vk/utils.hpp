#pragma once

#include "vk/device.hpp"
#include "vk/error.hpp"

namespace kzn::vk {

inline VkSemaphore create_semaphore(Device& device) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphore semaphore;
    auto result = vkCreateSemaphore(device.vk_device(), &semaphore_info, nullptr, &semaphore);
    VK_CHECK_MSG(result, "Failed to created synchronization semaphore!");
    return semaphore;
}

inline VkFence create_fence(Device& device, VkFenceCreateFlags flags = {}) {
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = flags;
    VkFence fence;
    auto result = vkCreateFence(device.vk_device(), &fence_info, nullptr, &fence);
    VK_CHECK_MSG(result, "Failed to created synchronization fence!");
    return fence;
}

inline void destroy_semaphore(Device& device, VkSemaphore semaphore) {
    vkDestroySemaphore(device.vk_device(), semaphore, nullptr);
}


inline void destroy_fence(Device& device, VkFence fence) {
    vkDestroyFence(device.vk_device(), fence, nullptr);
}


} // namespace kzn::vk
