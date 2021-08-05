#ifndef RENDERER
#define RENDERER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

namespace kzn
{

class Renderer
{
private:

    // Window related
    GLFWwindow* m_window;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    // Vulkan instance
    VkInstance m_instance;
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const bool ENABLE_VALIDATION_LAYERS = true;
    VkDebugUtilsMessengerEXT m_debug_messenger;

public:

    void run();
    
private:

    void init_window();

    void init_vulkan();

    void main_loop();

    void cleanup();

    // ======= Create functions ======= //

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
};

}

#endif // RENDERER