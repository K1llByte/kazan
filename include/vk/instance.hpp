#ifndef KZN_VK_INSTANCE_HPP
#define KZN_VK_INSTANCE_HPP

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <vector>

namespace kzn::vk
{
    // TODO: InstanceError, an exception for errors from
    // the instance or instance creations
    class Instance
    {
        friend class InstanceBuilder;
    public:
        ~Instance();

        VkInstance vk_instance() noexcept { return vkinstance; }
    
    private:
        Instance() = default;
        
    private:
        bool                     with_validation_layers;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkInstance               vkinstance;
    };

    class InstanceBuilder
    {
    public:
        InstanceBuilder() = default;
        ~InstanceBuilder() = default;

        InstanceBuilder& enable_validation_layers() noexcept;

        Instance build();

    private:
        bool                     with_validation_layers = false;
        std::vector<const char*> extensions;
        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    };
} // namespace kzn::vk

#endif // KZN_VK_INSTANCE_HPP