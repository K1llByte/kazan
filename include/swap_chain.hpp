#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H

#include "device.hpp"

namespace kzn
{

class SwapChain
{
public:

    // static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

private:

    Device&      _device;
    VkExtent2D   _window_extent;
    VkSurfaceKHR _surface;

    VkSwapchainKHR _swap_chain = VK_NULL_HANDLE;
    VkSwapchainKHR _old_swap_chain = VK_NULL_HANDLE;

    VkFormat   swap_chain_image_format;
    // VkFormat   swap_chain_depth_format;
    VkExtent2D swap_chain_extent;

    // std::vector<VkFramebuffer> swap_chain_framebuffers;
    // VkRenderPass               render_pass;

    // std::vector<VkImage>        depth_images;
    // std::vector<VkDeviceMemory> depth_image_memorys;
    // std::vector<VkImageView>    depth_image_views;
    std::vector<VkImage>        swap_chain_images;
    std::vector<VkImageView>    swap_chain_image_views;

    // std::vector<VkSemaphore> image_available_semaphores;
    // std::vector<VkSemaphore> render_finished_semaphores;
    // std::vector<VkFence>     in_flight_fences;
    // std::vector<VkFence>     images_in_flight;
    // size_t                   current_frame = 0;

public:

    SwapChain(Device& device, VkExtent2D win_extent, VkSurfaceKHR surface, VkSwapchainKHR old_swap_chain = VK_NULL_HANDLE);
    // SwapChain(const SwapChain&) = default;
    ~SwapChain();

    void init();
    void cleanup();

    bool initialized() const;

    VkSwapchainKHR swap_chain();

private:

    void create_swap_chain();

    // SwapChain support choosers
    VkSurfaceFormatKHR choose_surface_format(
            const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_present_mode(
        const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR &capabilities);

};

}

#endif // SWAP_CHAIN_H