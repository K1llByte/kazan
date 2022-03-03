#include "vk/swapchain.hpp"

#include "vk/utils.hpp"

namespace kzn::vk
{
    Swapchain::~Swapchain()
    {
        vkDestroySwapchainKHR(device->vk_device(), vkswapchain, nullptr);
        Log::debug("Swapchain destroyed");
    }

    SwapchainBuilder::SwapchainBuilder(Device* device, VkSurfaceKHR surface)
        : device(device) {}

    Swapchain SwapchainBuilder::build()
    {
        auto swapchain_support = device->swapchain_support_details();
        auto indices = device->queue_families();

        // 1. Create Swapchain.
        
        auto surface_format = swapchain_support.select_format();
        auto present_mode = swapchain_support.select_present_mode();
        auto extent = swapchain_support.select_extent(/* Pass in Window extent here */);

        // Select image count
        auto image_count = swapchain_support.capabilities.minImageCount + 1;
        // If there's on max image count (maxImageCount is 0) or the wanted image_count
        // exceeds maxImageCount capability, then just use maxImageCount
        if(swapchain_support.capabilities.maxImageCount > 0 &&
            image_count > swapchain_support.capabilities.maxImageCount)
        {
            image_count = swapchain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

        if(indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            // TODO: Change these two in the future to get this
            // from QueueFamiliesIndices struct
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        // No pre transform
        create_info.preTransform = swapchain_support.capabilities.currentTransform;
        // Composite blend with other windows
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        VkSwapchainKHR vkswapchain;
        auto result = vkCreateSwapchainKHR(device->vk_device(), &create_info, nullptr, &vkswapchain);
        VK_CHECK_MSG(result, "Failed to create swap chain!");
        Log::debug("Swapchain created");

        auto swapchain = Swapchain();
        swapchain.vkswapchain = vkswapchain;
        swapchain.device = device;
        return swapchain;
    }
} // namespace kzn::vk