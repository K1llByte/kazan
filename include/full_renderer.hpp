#ifndef RENDERER
#define RENDERER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

namespace tmp
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
    VkSwapchainKHR m_swap_chain;
    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;
    VkFormat m_swap_chain_image_format;
    VkExtent2D m_swap_chain_extent;
    std::vector<VkFramebuffer> m_swap_chain_framebuffers;

    // Graphics pipeline
    VkRenderPass m_render_pass;
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_graphics_pipeline;

    // Command buffers
    VkCommandPool m_command_pool;
    std::vector<VkCommandBuffer> m_command_buffers;

    // Unnamed
    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;
    size_t current_frame = 0;
    bool m_framebuffer_resized = false;

public:

    void run();

    // Graphics pipeline stuff
    static std::vector<char> read_file(const std::string& filename);
    
private:

    void init_window();

    void init_vulkan();

    void main_loop();

    void draw_frame();

    void cleanup();

    // ======= Create functions ======= //

    // Window stuff
    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

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

    void create_swap_chain();

    void cleanup_swap_chain();

    void create_image_views();

    void create_framebuffers();

    void recreate_swap_chain();

    // Graphics pipeline stuff
    void create_graphics_pipeline();

    VkShaderModule create_shader_module(const std::vector<char>& code);

    void create_render_pass();

    // Command buffer stuff
    void create_command_pool();

    void create_command_buffers();

    // Unnamed stuff
    void create_sync_objects();
};

}

#endif // RENDERER