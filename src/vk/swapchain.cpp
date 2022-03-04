#include "vk/swapchain.hpp"

#include "vk/utils.hpp"

namespace kzn::vk
{
    Swapchain::~Swapchain()
    {
        // Destroy Swapchain Image Views
        for(auto image_view : swapchain_image_views)
        {
            vkDestroyImageView(device->vk_device(), image_view, nullptr);
        }
        // Destroy Swapchain
        vkDestroySwapchainKHR(device->vk_device(), vkswapchain, nullptr);
        Log::debug("Swapchain destroyed");
    }

    SwapchainBuilder::SwapchainBuilder(Device* device, VkSurfaceKHR surface, VkExtent2D extent)
        : device(device), surface(surface), requested_extent(extent) {}

    Swapchain SwapchainBuilder::build()
    {
        Log::debug("({},{})", requested_extent.width, requested_extent.height);
        auto swapchain_support = device->swapchain_support_details();
        auto indices = device->queue_families();

        // 1. Create Swapchain.
        // 2. Retrieve VkImage handles
        // 3. Create VkImageView handles
        
        // 1. Create Swapchain //
        auto surface_format = swapchain_support.select_format();
        auto present_mode = swapchain_support.select_present_mode();
        auto extent = swapchain_support.select_extent(requested_extent);

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
        create_info.oldSwapchain = VK_NULL_HANDLE;


        VkSwapchainKHR vkswapchain = VK_NULL_HANDLE;
        Log::debug("debug 4");
        auto vkdevice = device->vk_device();
        
        auto result = vkCreateSwapchainKHR(vkdevice, &create_info, nullptr, &vkswapchain);
        VK_CHECK_MSG(result, "Failed to create swap chain!");
        // Log::debug("Swapchain created");
        Log::debug("debug 0");

        auto swapchain = Swapchain();
        swapchain.vkswapchain = vkswapchain;
        swapchain.device = device;

        // 2. Retrieve VkImage handles //
        // we only specified a minimum number of images in the swap chain, so the
        // implementation is allowed to create a swap chain with more, that's why
        // we'll first query the final number of images
        vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, nullptr);
        swapchain.swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, swapchain.swapchain_images.data());

        // 3. Create VkImageView handles //
        swapchain.swapchain_image_views.resize(image_count);
        for (size_t i = 0; i < image_count; ++i)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = swapchain.swapchain_images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = surface_format.format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;
            auto result = vkCreateImageView(device->vk_device(), &create_info, nullptr, &swapchain.swapchain_image_views[i]);
            VK_CHECK_MSG(result, "Failed to create image views!");
        }

        return swapchain;
    }
} // namespace kzn::vk