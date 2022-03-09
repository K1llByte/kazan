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

        VkSwapchainKHR vk_swapchain() noexcept { return vkswapchain; }

        const VkSurfaceFormatKHR get_surface_format() const noexcept { return surface_format; }
        const VkPresentModeKHR get_present_mode() const noexcept { return present_mode; }
        const VkExtent2D get_extent() const noexcept { return extent; }
        std::size_t num_images() const noexcept { return swapchain_image_views.size(); }
        const std::vector<VkImageView>& image_views() noexcept { return swapchain_image_views; }
        std::size_t current_index() const noexcept { return current_image_idx; }

        uint32_t acquire_next(VkSemaphore img_available_semaphore) noexcept;

    private:
        Swapchain() = default;

    private:
        VkSwapchainKHR           vkswapchain;
        std::vector<VkImage>     swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        VkSurfaceFormatKHR       surface_format;
        VkPresentModeKHR         present_mode;
        VkExtent2D               extent;
        // Since device must allways outlive the Swapchain for now
        // ill use a pointer reference FIXME: change to an alternative
        Device*                  device;
        uint32_t                 current_image_idx;
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