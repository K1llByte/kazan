#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <vector>

namespace kzn::vk
{
    class Instance
    {
        friend class InstanceBuilder;
    public:
        ~Instance();
    
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
        bool                     with_validation_layers;
        std::vector<const char*> extensions;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkInstance               vkinstance;
    };
}

#endif // INSTANCE_HPP