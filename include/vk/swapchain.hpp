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
        VkSwapchainKHR vkswapchain;
        // Since device must allways outlive the Swapchain for now
        // ill use a pointer reference FIXME: change to an alternative
        Device*        device;
    };

    class SwapchainBuilder
    {
    public:
        SwapchainBuilder(Device* device, VkSurfaceKHR surface);
        ~SwapchainBuilder() = default;

        Swapchain build();

    private:
        VkSurfaceKHR surface;
        Device*      device;
    };
} // namespace kzn::vk

#endif // KZN_VK_SWAPCHAIN_HPP