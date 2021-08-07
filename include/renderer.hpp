#ifndef RENDERER
#define RENDERER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>
#include <optional>

namespace kzn
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete()
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

class Renderer
{
private:

    // Window related
    GLFWwindow* m_window;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    VkSurfaceKHR m_surface;

    // Vulkan instance
    VkInstance m_instance;
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool ENABLE_VALIDATION_LAYERS = true;
    VkDebugUtilsMessengerEXT m_debug_messenger;

    // Device
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // SwapChain

public:

    void run();
    
private:

    void init_window();

    void init_vulkan();

    void main_loop();

    void cleanup();

    // ======= Create functions ======= //

    // Vulkan instance stuff
    void create_instance();

    bool check_validation_layer_support();

    std::vector<const char*> get_required_extensions();

    void setup_debug_messenger();

    VkResult CreateDebugUtilsMessenger(
        VkInstance                                instance,
        const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
        const VkAllocationCallbacks*              p_allocator,
        VkDebugUtilsMessengerEXT*                 p_debug_messenger);

    void DestroyDebugUtilsMessenger(
        VkInstance                   instance,
        VkDebugUtilsMessengerEXT     debug_messenger,
        const VkAllocationCallbacks* p_allocator);

    void populate_debug_messenger_create_info(
        VkDebugUtilsMessengerCreateInfoEXT& create_info);

    // Device stuff
    void pick_physical_device();

    bool is_device_suitable(VkPhysicalDevice physical_device);

    QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);

    void create_logical_device();

    void create_surface();

    bool check_device_extension_support(VkPhysicalDevice physical_device);

    // SwapChain stuff
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
};

}

#endif // RENDERER