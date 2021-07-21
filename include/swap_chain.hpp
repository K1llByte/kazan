#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include "device.hpp"

namespace kzn
{

class SwapChain
{
private:
    Device& m_device;
    VkExtent2D m_window_extent;
    VkSwapchainKHR m_swap_chain;
    
    std::vector<VkImage> swap_chain_images;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;

public:
    SwapChain(Device& device_ref, VkExtent2D window_extent);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    void operator=(const SwapChain &) = delete;
    
private:

    void create_swap_chain();
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

};

}

#endif // SWAP_CHAIN