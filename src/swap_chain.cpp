#include "swap_chain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace kzn
{

SwapChain::SwapChain(Device& device, VkExtent2D win_extent, VkSurfaceKHR surface, VkSwapchainKHR old_swap_chain)
    : _device{device},
    _window_extent{win_extent},
    _surface{surface},
    _swap_chain{old_swap_chain}
{
    
}


SwapChain::~SwapChain()
{

}


void SwapChain::init()
{
    // create_swap_chain();
    // create_image_views();
    // create_render_pass();
    // create_depth_resources();
    // create_framebuffers();
    // create_sync_objects();
}


void SwapChain::cleanup()
{

}


bool SwapChain::initialized() const
{
    return _swap_chain != VK_NULL_HANDLE;
}


VkSwapchainKHR SwapChain::swap_chain()
{
    return _swap_chain;
}


void SwapChain::create_swap_chain()
{
    SwapChainSupportDetails swap_chain_support = _device.get_swap_chain_support();

    // Choose settings for swapchain from SwapChainSupportDetails
    VkSurfaceFormatKHR surface_format = choose_surface_format(swap_chain_support.formats);
    VkPresentModeKHR present_mode = choose_present_mode(swap_chain_support.present_modes);
    VkExtent2D extent = choose_extent(swap_chain_support.capabilities);

    // Choose the number of images of the swapchain based on
    // the min and max of images allowed
    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if(swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount)
    {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    // Fill SwapChain create info
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = _surface;

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = _device._indices;
    uint32_t queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
        };

    if(indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;      // Optional
        create_info.pQueueFamilyIndices = nullptr;  // Optional
    }

    create_info.preTransform = swapChainSupport.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    // 'old_swap_chain' may be VK_NULL_HANDLE
    create_info.oldSwapchain = old_swap_chain;

    // Create SwapChain instance
    if(vkCreateSwapchainKHR(device.device(), &create_info, nullptr, &_swap_chain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device.device(), _swap_chain, &image_count, nullptr);
    swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device.device(), _swap_chain, &image_count, swap_chain_images.data());

    swap_chain_image_format = surface_format.format;
    swap_chain_extent = extent;
}


VkSurfaceFormatKHR choose_surface_format(
    const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for(const auto &available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }

    return available_formats[0];
}


VkPresentModeKHR choose_present_mode(
    const std::vector<VkPresentModeKHR> &available_present_modes)
{
    for (const auto &available_present_mode : available_present_modes)
    {
        if(available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            // std::cout << "Present mode: Mailbox" << std::endl;
            return available_present_mode;
        }
    }

    // std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        // TODO: Change for std::clamp instead of max and min
        VkExtent2D actual_extent = _window_extent;
        actual_extent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actual_extent.width));
        actual_extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actual_extent.height));
  
        return actual_extent;
    }
}

}
