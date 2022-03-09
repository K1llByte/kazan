#include "vk/utils.hpp"

#include "vk/device.hpp"

namespace kzn::vk
{
    VkSemaphore create_semaphore(Device& device)
    {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkSemaphore new_semaphore;
        auto result = vkCreateSemaphore(device.vk_device(), &semaphore_info, nullptr, &new_semaphore);
        VK_CHECK_MSG(result, "Failed to create semaphore");
        return new_semaphore;
    }

    VkFence create_fence(Device& device)
    {
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence new_fence;
        auto result = vkCreateFence(device.vk_device(), &fence_info, nullptr, &new_fence);
        VK_CHECK_MSG(result, "Failed to create fence");
        return new_fence;
    }

    void destroy_semaphore(Device& device, VkSemaphore semaphore) noexcept
    {
        vkDestroySemaphore(device.vk_device(), semaphore, nullptr);
    }

    void destroy_fence(Device& device, VkFence fence) noexcept
    {
        vkDestroyFence(device.vk_device(), fence, nullptr);
    }
}