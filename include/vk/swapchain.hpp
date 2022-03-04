#ifndef KZN_VK_SWAPCHAIN_HPP
#define KZN_VK_SWAPCHAIN_HPP

#include "vk/device.hpp"

namespace kzn::vk
{
    class Swapchain
    {
        friend class SwapchainBuilder;
    public:
        ~Swapchain();

    private:
        Swapchain() = default;

    private:
        VkSwapchainKHR           vkswapchain;
        std::vector<VkImage>     swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        // Since device must allways outlive the Swapchain for now
        // ill use a pointer reference FIXME: change to an alternative
        Device*                  device;
    };

    class SwapchainBuilder
    {
    public:
        SwapchainBuilder(Device* device, VkSurfaceKHR surface, VkExtent2D extent);
        ~SwapchainBuilder() = default;

        Swapchain build();

    private:
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkExtent2D   requested_extent;
        // Since device must allways outlive the Swapchain for now
        // ill use a pointer reference FIXME: change to an alternative
        Device*      device = nullptr;
    };
} // namespace kzn::vk

#endif // KZN_VK_SWAPCHAIN_HPP