#pragma once

#include "device.hpp"

namespace kzn::vk {

VkSemaphore create_semaphore(Device& device) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphore semaphore;
    auto result = vkCreateSemaphore(device.vk_device(), &semaphore_info, nullptr, &semaphore);
    VK_CHECK_MSG(result, "Failed to created synchronization semaphore!");
    return semaphore;
}

VkFence create_fence(Device& device) {
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    auto result = vkCreateFence(device.vk_device(), &fence_info, nullptr, &fence);
    VK_CHECK_MSG(result, "Failed to created synchronization fence!");
    return fence;
}

void destroy_semaphore(Device& device, VkSemaphore semaphore) {
    vkDestroySemaphore(device.vk_device(), semaphore, nullptr);
}


void destroy_fence(Device& device, VkFence fence) {
    vkDestroyFence(device.vk_device(), fence, nullptr);
}


} // namespace kzn::vk
