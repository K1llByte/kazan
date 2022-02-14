#include "swap_chain.hpp"

#include "logger.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <algorithm>

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
    if(initialized())
    {
        cleanup();
    }
}


void SwapChain::init()
{
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_depth_resources();
    create_framebuffers();
    create_sync_objects();
    Logger::debug("+ SwapChain created successfully");
}


void SwapChain::cleanup()
{
    // Destroy Color views
    for(auto image_view : _color_image_views)
    {
        vkDestroyImageView(_device.device(), image_view, nullptr);
    }
    _color_image_views.clear();

    // Destroy SwapChain
    if(_swap_chain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(_device.device(), _swap_chain, nullptr);
        _swap_chain = nullptr;
    }

    // Destroy Depth images, views and memories
    for(size_t i = 0; i < _depth_images.size(); ++i)
    {
        vkDestroyImageView(_device.device(), _depth_image_views[i], nullptr);
        vkDestroyImage(_device.device(), _depth_images[i], nullptr);
        vkFreeMemory(_device.device(), _depth_image_memorys[i], nullptr);
    }

    // Destroy Framebuffers
    for(auto framebuffer : _swap_chain_framebuffers)
    {
        vkDestroyFramebuffer(_device.device(), framebuffer, nullptr);
    }

    // Destroy RenderPass
    vkDestroyRenderPass(_device.device(), _render_pass, nullptr);

    // Destroy Synchronization objects
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(_device.device(), _render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(_device.device(), _image_available_semaphores[i], nullptr);
        vkDestroyFence(_device.device(), _in_flight_fences[i], nullptr);
    }
    Logger::debug("- SwapChain destroyed successfully");
}


bool SwapChain::initialized() const
{
    return _swap_chain != VK_NULL_HANDLE;
}


VkSwapchainKHR SwapChain::swap_chain() const
{
    return _swap_chain;
}


VkRenderPass SwapChain::render_pass() const
{
    return _render_pass;
}


VkFramebuffer SwapChain::framebuffer(uint32_t image_index) const
{
    return _swap_chain_framebuffers[image_index];
}


VkExtent2D SwapChain::swap_chain_extent() const
{
    return _swap_chain_extent;
}


float SwapChain::extent_aspect_ratio() const
{
    return static_cast<float>(_swap_chain_extent.width) / static_cast<float>(_swap_chain_extent.height);
    // return _window_extent.width / _window_extent.height;
}


size_t SwapChain::image_count() const
{
    return _color_images.size();
}


VkResult SwapChain::acquire_next_image(uint32_t* image_index)
{
    vkWaitForFences(
        _device.device(),
        1,
        &_in_flight_fences[_current_frame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        _device.device(),
        _swap_chain,
        std::numeric_limits<uint64_t>::max(),
        _image_available_semaphores[_current_frame], // must be an unsignaled semaphore
        VK_NULL_HANDLE,
        image_index);

    return result;
}


VkResult SwapChain::submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* image_index)
{
    if(_images_in_flight[*image_index] != VK_NULL_HANDLE)
    {
        vkWaitForFences(_device.device(), 1, &_images_in_flight[*image_index], VK_TRUE, UINT64_MAX);
    }
    _images_in_flight[*image_index] = _in_flight_fences[_current_frame];

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
        _image_available_semaphores[_current_frame]
    };
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = buffers;

    VkSemaphore signal_semaphores[] = {
        _render_finished_semaphores[_current_frame]
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(_device.device(), 1, &_in_flight_fences[_current_frame]);
    if(vkQueueSubmit(_device.graphics_queue(), 1, &submit_info, _in_flight_fences[_current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &_swap_chain;

    present_info.pImageIndices = image_index;

    auto result = vkQueuePresentKHR(_device.present_queue(), &present_info);

    _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}


bool SwapChain::compare_formats(const SwapChain &swap_chain) const
{
    return swap_chain._depth_format == _depth_format &&
        swap_chain._image_format == _image_format;
}


void SwapChain::create_swap_chain()
{
    SwapChainSupportDetails swap_chain_support = _device.query_swap_chain_support(_surface); //_device._swap_chain_support;

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
    VkResult res = vkCreateSwapchainKHR(_device.device(), &create_info, nullptr, &_swap_chain);
    // if(vkCreateSwapchainKHR(_device.device(), &create_info, nullptr, &_swap_chain) != VK_SUCCESS)
    if(res != VK_SUCCESS)
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
    // TODO: Change '_depth_format' to accept stencil data
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
    const size_t image_count = _color_images.size();
    _depth_images.resize(image_count);
    _depth_image_memorys.resize(image_count);
    _depth_image_views.resize(image_count);

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


void SwapChain::create_framebuffers()
{
    const size_t image_count = _color_images.size();
    _swap_chain_framebuffers.resize(image_count);

    for(size_t i = 0; i < image_count; ++i)
    {
        std::array<VkImageView, 2> attachments = {
            _color_image_views[i],
            _depth_image_views[i]
            };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = _render_pass;
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = _swap_chain_extent.width;
        framebuffer_info.height = _swap_chain_extent.height;
        framebuffer_info.layers = 1;

        if(vkCreateFramebuffer(
            _device.device(),
            &framebuffer_info,
            nullptr,
            &_swap_chain_framebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}


void SwapChain::create_sync_objects()
{
    const size_t image_count = _color_images.size();
    _image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    _images_in_flight.resize(image_count, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if(vkCreateSemaphore(_device.device(), &semaphore_info, nullptr, &_image_available_semaphores[i]) != VK_SUCCESS
            || vkCreateSemaphore(_device.device(), &semaphore_info, nullptr, &_render_finished_semaphores[i]) != VK_SUCCESS
            || vkCreateFence(_device.device(), &fence_info, nullptr, &_in_flight_fences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
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
        VkExtent2D actual_extent = _window_extent;

        actual_extent.width = std::clamp(
            actual_extent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );
        actual_extent.height = std::clamp(
            actual_extent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );

        return actual_extent;
    }
}

}
