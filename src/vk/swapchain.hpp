#pragma once

#include "core/log.hpp"
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

    [[nodiscard]]
    constexpr VkSwapchainKHR vk_swapchain() {
        return m_vk_swapchain;
    }
    [[nodiscard]]
    constexpr VkFormat image_format() const {
        return m_surface_format.format;
    }
    [[nodiscard]]
    constexpr VkExtent2D extent() const {
        return m_extent;
    }
    [[nodiscard]]
    constexpr std::span<VkImage> images() {
        return std::span{m_images.data(), m_images.size()};
    }
    [[nodiscard]]
    constexpr std::span<VkImageView> image_views() {
        return std::span{m_image_views.data(), m_image_views.size()};
    }
    [[nodiscard]]
    constexpr size_t current_index() const {
        return m_current_index;
    }

    std::optional<uint32_t> acquire_next(VkSemaphore signal_semaphore);
    void recreate(VkExtent2D new_extent);

private:
    Device& m_device;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_vk_swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR m_surface_format;
    VkPresentModeKHR m_present_mode;
    VkExtent2D m_extent;
    uint32_t m_image_count;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;
    uint32_t m_current_index = 0;
};

} // namespace kzn::vk
