#ifndef ENGINE_H
#define ENGINE_H

#include "types.hpp"

#include <vector>

namespace kzn
{

class Engine
{
public:

    bool _is_initialized{ false };
    int  _frame_number{ 0 };

    VkExtent2D _window_extent{ 1700 , 900 };

    struct GLFWwindow* _window{ nullptr };

    VkInstance               _instance; // Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice         _physical_device; // GPU chosen as the default device
    VkDevice                 _device; // Vulkan device for commands
    VkSurfaceKHR             _surface; // Vulkan window surface

    VkSwapchainKHR           _swapchain;
    VkFormat                 _swapchain_image_format;
    std::vector<VkImage>     _swapchain_images;
    std::vector<VkImageView> _swapchain_image_views;

    VkQueue  _graphics_queue;
    uint32_t _graphics_queue_family;

    VkCommandPool   _command_pool;
    VkCommandBuffer _main_command_buffer;

    VkRenderPass _render_pass;
    std::vector<VkFramebuffer> _framebuffers;

    VkSemaphore _present_semaphore;
    VkSemaphore _render_semaphore;
	VkFence _render_fence;

public:

    // Initializes necessary objects
    void init();

    // Destroys every objects
    void cleanup();

    // Draw loop
    void draw();

    // Run main loop
    void run();

private:

    void init_vulkan();

    void init_swapchain();

    void init_commands();

    void init_default_renderpass();

	void init_framebuffers();

    void init_sync_structures();
};

}

#endif // ENGINE_H