#ifndef KZN_VK_INSTANCE_HPP
#define KZN_VK_INSTANCE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace kzn::vk
{
    class Instance
    {
        friend class InstanceBuilder;
    public:
        ~Instance();

        VkInstance vk_instance() noexcept { return vkinstance; }
        const std::vector<const char*>& get_validation_layers() const noexcept;

        VkSurfaceKHR create_surface(GLFWwindow* glfw_window);

    
    private:
        Instance() = default;
        
    private:
        bool                     with_validation_layers;
        std::vector<const char*> validation_layers;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkInstance               vkinstance;
        VkSurfaceKHR             surface = VK_NULL_HANDLE;
    };

    class InstanceBuilder
    {
    public:
        InstanceBuilder() = default;
        ~InstanceBuilder() = default;

        InstanceBuilder& enable_validation_layers() noexcept;
        InstanceBuilder& set_extensions(const std::vector<const char*>&) noexcept;

        Instance build();

    private:
        bool                     with_validation_layers = false;
        std::vector<const char*> validation_layers{"VK_LAYER_KHRONOS_validation"};
        std::vector<const char*> instance_extensions;
        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    };
} // namespace kzn::vk

#endif // KZN_VK_INSTANCE_HPP