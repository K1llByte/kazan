#pragma once

#include "vk/device.hpp"
#include "vk/surface.hpp"

#include <span>

namespace kzn::vk {

class Swapchain {
public:
    // Ctor
    Swapchain(Device& device, Surface& surface, VkExtent2D extent);
    // Copy
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    // Move
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;
    // Dtor
    ~Swapchain();

    constexpr VkFormat image_format() const { return m_surface_format.format; }
    constexpr VkExtent2D extent() const { return m_extent; }
    constexpr std::span<VkImage> image() { return std::span{m_images.data(), m_images.size()}; }
    constexpr std::span<VkImageView> image_views() { return std::span{m_image_views.data(), m_image_views.size()}; }

private:
    Device&                  m_device;
    VkSwapchainKHR           m_vk_swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR       m_surface_format;
    VkPresentModeKHR         m_present_mode;
    VkExtent2D               m_extent;
    uint32_t                 m_image_count;
    std::vector<VkImage>     m_images;
    std::vector<VkImageView> m_image_views;
};

} // namespace kzn::vk
