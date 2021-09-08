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
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_depth_resources();
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
    SwapChainSupportDetails swap_chain_support = _device._swap_chain_support;

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

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    // 'old_swap_chain' may be VK_NULL_HANDLE
    create_info.oldSwapchain = _old_swap_chain;

    // Create SwapChain instance
    if(vkCreateSwapchainKHR(_device.device(), &create_info, nullptr, &_swap_chain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(_device.device(), _swap_chain, &image_count, nullptr);
    _color_images.resize(image_count);
    vkGetSwapchainImagesKHR(_device.device(), _swap_chain, &image_count, _color_images.data());

    _image_format = surface_format.format;
    _swap_chain_extent = extent;
}


void SwapChain::create_image_views()
{
    _color_image_views.resize(_color_images.size());
    for(size_t i = 0; i < _color_images.size(); ++i)
    {
        VkImageViewCreateInfo _view_info{};
        _view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        _view_info.image = _color_images[i];
        _view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        _view_info.format = _image_format;
        _view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        _view_info.subresourceRange.baseMipLevel = 0;
        _view_info.subresourceRange.levelCount = 1;
        _view_info.subresourceRange.baseArrayLayer = 0;
        _view_info.subresourceRange.layerCount = 1;
  
        if (vkCreateImageView(_device.device(), &_view_info, nullptr, &_color_image_views[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}


void SwapChain::create_render_pass()
{
    // Depth attachment
    _depth_format = VK_FORMAT_D32_SFLOAT;
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = _depth_format; //findDepthFormat();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Color attachment
    VkAttachmentDescription color_attachment{};
    color_attachment.format = _image_format; //getSwapChainImageFormat();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.dstSubpass = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                             | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                            | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                            | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    std::array<VkAttachmentDescription, 2> attachments{ color_attachment, depth_attachment };
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if(vkCreateRenderPass(_device.device(), &render_pass_info, nullptr, &_render_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}


void SwapChain::create_depth_resources()
{
    size_t image_count = _color_images.size();
    _depth_images.resize(image_count);
    _depth_image_memorys.resize(image_count);
    _depth_image_views.resize(image_count);

    // TODO: change to size_t
    for(size_t i = 0; i < _depth_images.size(); ++i)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = _swap_chain_extent.width;
        image_info.extent.height = _swap_chain_extent.height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = _depth_format;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.flags = 0;

        _device.create_image_with_info(
            image_info,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            _depth_images[i],
            _depth_image_memorys[i]);

        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = _depth_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = _depth_format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if(vkCreateImageView(_device.device(), &view_info, nullptr, &_depth_image_views[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}


VkSurfaceFormatKHR SwapChain::choose_surface_format(
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


VkPresentModeKHR SwapChain::choose_present_mode(
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


VkExtent2D SwapChain::choose_extent(const VkSurfaceCapabilitiesKHR &capabilities)
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
