#include "vk/swapchain.hpp"

#include "vk/utils.hpp"
#include "vk/error.hpp"

namespace kzn::vk
{
    Swapchain::~Swapchain()
    {
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

        // 2.1 Create new swapchain //
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

        VkSwapchainKHR old_vkswapchain = vkswapchain;
        create_info.preTransform = swapchain_support.capabilities.currentTransform;
        // NOTE: if we want window to be transparent
        // this needs to change
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = old_vkswapchain;

        auto result = vkCreateSwapchainKHR(device->vk_device(), &create_info, nullptr, &vkswapchain);
        VK_CHECK_MSG(result, "Failed to create swap chain!");

        // Not sure if i need to retrieve image_count
        // FIXME: Might be a problem here
        // vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, nullptr);
        // swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(device->vk_device(), vkswapchain, &image_count, swapchain_images.data());

        // 4. Cleanup old swapchain image views //
        // Destroy Swapchain
        vkDestroySwapchainKHR(device->vk_device(), old_vkswapchain, nullptr);
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

        return swapchain;
    }
} // namespace kzn::vk