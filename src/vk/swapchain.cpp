#include "vk/swapchain.hpp"

#include "vk/utils.hpp"
#include "vk/error.hpp"

namespace kzn::vk
{
    Swapchain::~Swapchain()
    {
        // Destroy Swapchain Depth Image Views
        for(auto depth_view : swapchain_depth_views)
        {
            vkDestroyImageView(device->vk_device(), depth_view, nullptr);
        }
        // Destroy Swapchain Depth Images
        for(std::size_t i = 0; i < swapchain_depths.size(); ++i)
        {
		    vmaDestroyImage(device->allocator(), swapchain_depths[i], depth_allocation[i]);
        }
        // Destroy Swapchain Color Image Views
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
        auto old_swapchain_image_views = swapchain_image_views;
        auto old_swapchain_depth_views = swapchain_depth_views;
        auto old_swapchain_depths = swapchain_depths;
        auto old_depth_allocation = depth_allocation;
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

        // 2.2 Create new ImageViews //
        for(std::size_t i = 0; i < swapchain_image_views.size(); ++i)
        {
            VkImageViewCreateInfo img_view_create_info{};
            img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            img_view_create_info.image = swapchain_images[i];
            img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            img_view_create_info.format = surface_format.format;
            img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            img_view_create_info.subresourceRange.baseMipLevel = 0;
            img_view_create_info.subresourceRange.levelCount = 1;
            img_view_create_info.subresourceRange.baseArrayLayer = 0;
            img_view_create_info.subresourceRange.layerCount = 1;

            result = vkCreateImageView(device->vk_device(), &img_view_create_info, nullptr, &swapchain_image_views[i]);
            VK_CHECK_MSG(result, "Failed to create image views!");
        }

        // 3. Create Depth Resources //
        for(size_t i = 0; i < swapchain_depths.size(); ++i)
        {
            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.pNext = nullptr;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.format = VK_FORMAT_D32_SFLOAT;
            image_info.extent = VkExtent3D{
                extent.width,
                extent.height,
                1
            };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            
            //for the depth image, we want to allocate it from GPU local memory
            VmaAllocationCreateInfo depth_img_alloc_info{};
            depth_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            depth_img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            result = vmaCreateImage(device->allocator(), &image_info, &depth_img_alloc_info, &swapchain_depths[i], &depth_allocation[i], nullptr);
            VK_CHECK_MSG(result, "Failed to create depth images!");

            // Build an ImageView for the depth image to use for rendering
	        VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = swapchain_depths[i];
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format = VK_FORMAT_D32_SFLOAT;
            image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount = 1;
            // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
            result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &swapchain_depth_views[i]);
	        VK_CHECK_MSG(result, "Failed to create depth image views!");
        }

        // 4. Cleanup old swapchain image views //
        
        // Destroy Swapchain Depth Image Views
        for(auto old_depth_view : old_swapchain_depth_views)
        {
            vkDestroyImageView(device->vk_device(), old_depth_view, nullptr);
        }
        // Destroy Swapchain Depth Images
        for(std::size_t i = 0; i < old_swapchain_depths.size(); ++i)
        {
		    vmaDestroyImage(device->allocator(), old_swapchain_depths[i], old_depth_allocation[i]);
        }

        // Destroy ImageViews
        for (size_t i = 0; i < old_swapchain_image_views.size(); i++)
        {
            vkDestroyImageView(device->vk_device(), old_swapchain_image_views[i], nullptr);
        }
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

        // 3. Create VkImageView handles //
        swapchain.swapchain_image_views.resize(image_count);
        for(size_t i = 0; i < image_count; ++i)
        {
            VkImageViewCreateInfo img_view_create_info{};
            img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            img_view_create_info.image = swapchain.swapchain_images[i];
            img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            img_view_create_info.format = swapchain.surface_format.format;
            img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            img_view_create_info.subresourceRange.baseMipLevel = 0;
            img_view_create_info.subresourceRange.levelCount = 1;
            img_view_create_info.subresourceRange.baseArrayLayer = 0;
            img_view_create_info.subresourceRange.layerCount = 1;
            result = vkCreateImageView(device->vk_device(), &img_view_create_info, nullptr, &swapchain.swapchain_image_views[i]);
            VK_CHECK_MSG(result, "Failed to create image views!");
        }

        // 4. Create Depth Resources //
        // 4.1 Create Depth Images  //
        const size_t num_depths = 1; 
        swapchain.swapchain_depths.resize(num_depths);
        swapchain.depth_allocation.resize(num_depths);
        swapchain.swapchain_depth_views.resize(num_depths);
        for(size_t i = 0; i < num_depths; ++i)
        {
            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.pNext = nullptr;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.format = VK_FORMAT_D32_SFLOAT;
            image_info.extent = VkExtent3D{
                swapchain.extent.width,
                swapchain.extent.height,
                1
            };
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            
            //for the depth image, we want to allocate it from GPU local memory
            VmaAllocationCreateInfo depth_img_alloc_info{};
            depth_img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            depth_img_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            result = vmaCreateImage(device->allocator(), &image_info, &depth_img_alloc_info, &swapchain.swapchain_depths[i], &swapchain.depth_allocation[i], nullptr);
            VK_CHECK_MSG(result, "Failed to create depth images!");

            // Build an ImageView for the depth image to use for rendering
	        VkImageViewCreateInfo image_view_info{};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = swapchain.swapchain_depths[i];
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format = VK_FORMAT_D32_SFLOAT;
            image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount = 1;
            // vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);
            result = vkCreateImageView(device->vk_device(), &image_view_info, nullptr, &swapchain.swapchain_depth_views[i]);
	        VK_CHECK_MSG(result, "Failed to create depth image views!");
        }

        return swapchain;
    }
} // namespace kzn::vk