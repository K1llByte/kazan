#include "vk/swapchain.hpp"

#include "vk/error.hpp"
#include "core/log.hpp"

namespace kzn::vk {

Swapchain::Swapchain(Device& device, Surface& surface, VkExtent2D requested_extent)
    : m_device(device)
{

    const auto& support = m_device.swapchain_support();
    const auto& queue_families = m_device.queue_families();

    // 1. Create Swapchain //
    m_surface_format = support.select_format();
    m_present_mode = support.select_present_mode();
    m_extent = support.select_extent(requested_extent);

    // Select image count
    uint32_t request_image_count = support.capabilities.minImageCount + 1;
    // If there's no max image count (maxImageCount is 0) or the wanted image_count
    // exceeds maxImageCount capability, then just use maxImageCount
    if(support.capabilities.maxImageCount > 0)
    {
        request_image_count = std::min(request_image_count, support.capabilities.maxImageCount);
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface.vk_surface();
    create_info.minImageCount = request_image_count;
    create_info.imageFormat = m_surface_format.format;
    create_info.imageColorSpace = m_surface_format.colorSpace;
    create_info.imageExtent = m_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t queue_families_indices[] = {
        queue_families.graphics_family.value(),
        queue_families.present_family.value()
    };

    if(queue_families.graphics_family != queue_families.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        // TODO: Change these two in the future to get this
        // from QueueFamiliesIndices struct
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_families_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0; // Optional
        create_info.pQueueFamilyIndices = nullptr; // Optional
    }

    // No pre transform
    create_info.preTransform = support.capabilities.currentTransform;
    // Composite blend with other windows
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = m_present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    auto result = vkCreateSwapchainKHR(device.vk_device(), &create_info, nullptr, &m_vk_swapchain);
    VK_CHECK_MSG(result, "Failed to create swap chain!");

    // 2. Retrieve VkImage handles //
    // we only specified a minimum number of images in the swap chain, so the
    // implementation is allowed to create a swapchain with more, that's why
    // we'll first query the final number of images.
    vkGetSwapchainImagesKHR(m_device.vk_device(), m_vk_swapchain, &m_image_count, nullptr);
    m_images.resize(m_image_count);
    vkGetSwapchainImagesKHR(m_device.vk_device(), m_vk_swapchain, &m_image_count, m_images.data());
    Log::trace("Swapchain created");

    // 3. Create VkImageView's //
    m_image_views.resize(m_image_count);
    for(size_t i = 0; i < m_image_count; ++i) {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = m_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_surface_format.format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        auto result = vkCreateImageView(m_device.vk_device(), &create_info, nullptr, &m_image_views[i]);
        VK_CHECK_MSG(result, "Failed to create image views!");
    }
}


Swapchain::~Swapchain() {
    // Destroy Image Views
    for(auto image_view : m_image_views) {
        vkDestroyImageView(m_device.vk_device(), image_view, nullptr);
    }
    
    // Destroy Swapchain
    vkDestroySwapchainKHR(m_device.vk_device(), m_vk_swapchain, nullptr);
    Log::trace("Swapchain destroyed");
}


uint32_t Swapchain::acquire_next(VkSemaphore signal_semaphore) {
    auto result = vkAcquireNextImageKHR(
        m_device.vk_device(),
        m_vk_swapchain,
        UINT64_MAX,
        signal_semaphore,
        VK_NULL_HANDLE,
        &m_current_index
    );
    return m_current_index;
}

}