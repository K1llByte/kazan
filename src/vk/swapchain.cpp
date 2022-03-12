#include "vk/swapchain.hpp"

#include "vk/utils.hpp"
#include "vk/error.hpp"

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

    uint32_t Swapchain::acquire_next(VkSemaphore img_available_semaphore)
    {
        auto result = vkAcquireNextImageKHR(
            device->vk_device(),
            vkswapchain,
            UINT64_MAX,
            img_available_semaphore,
            VK_NULL_HANDLE,
            &current_image_idx
        );
        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            throw SwapchainResized();
        }
        else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw AcquireImageError();
        }
        return current_image_idx;
    }

    void Swapchain::recreate(VkExtent2D new_extent)
    {
        // 1. Wait until device is idle //
        device->wait_idle();

        // 2. Cleanup old swapchain //
        // Destroy ImageViews
        for (size_t i = 0; i < swapchain_image_views.size(); i++)
        {
            vkDestroyImageView(device->vk_device(), swapchain_image_views[i], nullptr);
        }
        // Destroy Swapchain
        vkDestroySwapchainKHR(device->vk_device(), vkswapchain, nullptr);

        // 3.1 Create new swapchain //
        auto swapchain_support = device->query_swapchain_support(surface);
        
        surface_format = swapchain_support.select_format();
        // swapchain.present_mode = swapchain_support.select_present_mode(present_mode);
        extent = swapchain_support.select_extent(new_extent);

        auto image_count = static_cast<uint32_t>(swapchain_images.size());

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = surface;

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        Log::warning("Create Swapchain: ({},{})", create_info.imageExtent.width, create_info.imageExtent.height);
        auto indices = device->queue_families();
        uint32_t queue_family_indices[] = {
            indices.graphics_family.value(),
            indices.present_family.value()
        };

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        create_info.preTransform = swapchain_support.capabilities.currentTransform;
        // NOTE: if we want window to be transparent
        // this needs to change
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        auto result = vkCreateSwapchainKHR(device->vk_device(), &create_info, nullptr, &vkswapchain);
        VK_CHECK_MSG(result, "Failed to create swap chain!");

        // Not sure if i need to retrieve image_count
        // FIXME: Might be a problem here
        // vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, nullptr);
        // swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, swapchain_images.data());

        // 3.2 Create new ImageViews //
        for(std::size_t i = 0; i < swapchain_image_views.size(); ++i)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = swapchain_images[i];
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

            auto result = vkCreateImageView(device->vk_device(), &create_info, nullptr, &swapchain_image_views[i]);
            VK_CHECK_MSG(result, "Failed to create image views!");
        }
    }

    SwapchainBuilder::SwapchainBuilder(Device* device, VkSurfaceKHR surface, VkExtent2D extent)
        : device(device), surface(surface), requested_extent(extent) {}

    SwapchainBuilder& SwapchainBuilder::set_present_mode(VkPresentModeKHR _present_mode) noexcept
    {
        present_mode = _present_mode;
        return *this;
    }

    Swapchain SwapchainBuilder::build()
    {
        auto swapchain = Swapchain();

        auto swapchain_support = device->swapchain_support();
        auto indices = device->queue_families();

        // 1. Create Swapchain.
        // 2. Retrieve VkImage handles
        // 3. Create VkImageView handles
        
        // 1. Create Swapchain //
        swapchain.surface_format = swapchain_support.select_format();
        // swapchain.present_mode = swapchain_support.select_present_mode();
        swapchain.present_mode = present_mode;
        swapchain.extent = swapchain_support.select_extent(requested_extent);

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
        create_info.imageFormat = swapchain.surface_format.format;
        create_info.imageColorSpace = swapchain.surface_format.colorSpace;
        create_info.imageExtent = swapchain.extent;
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
        create_info.presentMode = swapchain.present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        VkSwapchainKHR vkswapchain = VK_NULL_HANDLE;
        auto vkdevice = device->vk_device();

        auto result = vkCreateSwapchainKHR(vkdevice, &create_info, nullptr, &vkswapchain);
        VK_CHECK_MSG(result, "Failed to create swap chain!");

        swapchain.vkswapchain = vkswapchain;
        swapchain.device = device;
        swapchain.surface = surface;

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
            create_info.format = swapchain.surface_format.format;
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