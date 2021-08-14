#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include "device.hpp"

namespace kzn
{

class SwapChain
{

};


class SwapChainBuilder
{
public:

    Device& m_device;
    VkSwapchainKHR m_swap_chain;
    std::vector<VkImage> m_swap_chain_images;
    // std::vector<VkImageView> m_swap_chain_image_views;
    // VkFormat m_swap_chain_image_format;
    // VkExtent2D m_swap_chain_extent;
    // std::vector<VkFramebuffer> m_swap_chain_framebuffers;

public:

    SwapChainBuilder(Device& device);

    SwapChain build()
};

}

#endif // SWAP_CHAIN