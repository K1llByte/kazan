#include "swap_chain.hpp"

#include <cstdint>
#include <algorithm>

namespace kzn
{
    SwapChain::SwapChain(Device &device_ref, VkExtent2D window_extent)
        : m_device{device_ref}, m_window_extent{window_extent}
    {
        create_swap_chain();
        create_image_views();
    }

    SwapChain::~SwapChain()
    {
        for (auto& image_view : m_swap_chain_image_views)
        {
            vkDestroyImageView(m_device.device(), image_view, nullptr);
        }

        vkDestroySwapchainKHR(m_device.device(), m_swap_chain, nullptr);
    }

    void SwapChain::create_swap_chain()
    {
        SwapChainSupportDetails swap_chain_support = m_device.get_swap_chain_support();

        VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
        VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
        VkExtent2D extent = choose_swap_extent(swap_chain_support.capabilities);

        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
        {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = m_device.surface();

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = m_device.get_physical_queue_families();
        uint32_t queue_family_indices[2] = {
            indices.graphics_family.value(),
            indices.present_family.value()};

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices = nullptr;
        }

        create_info.preTransform = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device.device(), &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
            throw std::runtime_error("failed to create swap chain!");

        vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &image_count, nullptr);
        m_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &image_count, m_swap_chain_images.data());

        m_swap_chain_image_format = surface_format.format;
        m_swap_chain_extent = extent;
    }

    VkSurfaceFormatKHR SwapChain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats)
    {
        for (const auto &avaiable_format : available_formats)
            if (avaiable_format.format == VK_FORMAT_B8G8R8A8_SRGB && avaiable_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return avaiable_format;

        return available_formats[0];
    }

    VkPresentModeKHR SwapChain::choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes)
    {
        for (const auto& present_mode : available_present_modes)
            if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return present_mode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actual_extent = {
                std::clamp(m_window_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                std::clamp(m_window_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};

            return actual_extent;
        }
    }

    void SwapChain::create_image_views()
    {
        m_swap_chain_image_views.resize(m_swap_chain_images.size());
        for (size_t i = 0; i < m_swap_chain_images.size(); i++)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = m_swap_chain_images[i];
            
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = m_swap_chain_image_format;

            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device.device(), &create_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create image views!");
        }
    }
}