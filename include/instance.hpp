#ifndef INSTANCE
#define INSTANCE

/**
 * The vulkan headers already define
 * preprocessor conditional compilation
 * rules for c++ compatability
 */

// extern "C"
// {
#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif
// }

#include <vector>

namespace kzn
{

struct Instance
{
    VkInstance m_instance;
};

class InstanceBuilder
{
private:

    VkApplicationInfo m_app_info{};
    VkInstanceCreateInfo m_create_info{};
    std::vector<const char*> m_extensions{};

    std::vector<const char*> m_validation_layers = {};
    // VkDebugUtilsMessengerEXT m_debug_messenger;

public:

    InstanceBuilder();

    ~InstanceBuilder() = default;

    // Setters
    InstanceBuilder& enable_extensions(const std::vector<const char*>& extensions);

    InstanceBuilder& enable_validation_layers();

    // Build
    Instance build();
};

} // kzn

#endif // INSTANCE