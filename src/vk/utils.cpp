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

    VkFence create_fence(Device& device, bool signaled)
    {
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        VkFence new_fence;
        auto result = vkCreateFence(device.vk_device(), &fence_info, nullptr, &new_fence);
        VK_CHECK_MSG(result, "Failed to create fence");
        return new_fence;
    }

    void destroy_semaphore(Device& device, VkSemaphore semaphore) noexcept {
        vkDestroySemaphore(device.vk_device(), semaphore, nullptr);
    }

    void destroy_fence(Device& device, VkFence fence) noexcept
    {
        vkDestroyFence(device.vk_device(), fence, nullptr);
    }

    VkViewport create_viewport(VkExtent2D extent)
    {
        return VkViewport{
            .x = 0,
            .y = 0,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0,
            .maxDepth = 1.0,
        };
    }

    VkRect2D create_scissor(VkExtent2D extent)
    {
        return VkRect2D{
            {0, 0},
            extent
        };
    }

    ////////// Vulkan image utils //////////

    // VkImage create_image(Device& device) {

    // }

    VkImageView create_image_view(
        Device& device,
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspect_mask)
    {
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = format;
        view_info.subresourceRange.aspectMask = aspect_mask;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkImageView image_view;
        auto result = vkCreateImageView(device.vk_device(), &view_info, nullptr, &image_view);
        VK_CHECK_MSG(result, "Failed to create image view");
        return image_view;
    }

    VkSampler create_sampler(Device& device) {
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        // Get PhysicalDeviceProperties
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device.vk_physical_device(), &properties);
        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        VkSampler sampler;
        auto result = vkCreateSampler(device.vk_device(), &sampler_info, nullptr, &sampler);
        VK_CHECK_MSG(result, "Failed to create sampler");
        return sampler;
    }

    // void destroy_image(Device& device) {

    // }

    void destroy_image_view(Device& device, VkImageView image_view) {
        vkDestroyImageView(device.vk_device(), image_view, nullptr);
    }

    void destroy_sampler(Device& device, VkSampler sampler) {
        vkDestroySampler(device.vk_device(), sampler, nullptr);
    }

}