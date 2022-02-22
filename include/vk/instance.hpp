#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <vector>

namespace kzn::vk
{
    struct Instance
    {

    };

    class InstanceBuilder
    {
    public:
        InstanceBuilder() = default;
        ~InstanceBuilder();

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