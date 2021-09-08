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

    VkFormat   _image_format;
    VkFormat   _depth_format;
    VkExtent2D _swap_chain_extent;

    std::vector<VkFramebuffer> _swap_chain_framebuffers;
    VkRenderPass               _render_pass;

    std::vector<VkImage>        _depth_images;
    std::vector<VkDeviceMemory> _depth_image_memorys;
    std::vector<VkImageView>    _depth_image_views;
    std::vector<VkImage>        _color_images;
    std::vector<VkImageView>    _color_image_views;

    // std::vector<VkSemaphore> _image_available_semaphores;
    // std::vector<VkSemaphore> _render_finished_semaphores;
    // std::vector<VkFence>     _in_flight_fences;
    // std::vector<VkFence>     _images_in_flight;
    // size_t                   _current_frame = 0;

public:

    SwapChain(Device& device, VkExtent2D win_extent, VkSurfaceKHR surface, VkSwapchainKHR old_swap_chain = VK_NULL_HANDLE);
    // SwapChain(const SwapChain&) = default;
    ~SwapChain();

    void init();
    void cleanup();

    bool initialized() const;

    VkSwapchainKHR swap_chain();

    // VkFormat find_depth_format();

private:

    void create_swap_chain();
    void create_image_views();
    void create_render_pass();
    void create_depth_resources();
    void create_framebuffers();

    // SwapChain support choosers
    VkSurfaceFormatKHR choose_surface_format(
            const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_present_mode(
        const std::vector<VkPresentModeKHR> &available_present_modes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR &capabilities);

};

}

#endif // SWAP_CHAIN_H