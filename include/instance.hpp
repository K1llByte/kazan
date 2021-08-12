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

class Instance
{
public:

    VkInstance m_instance;
    bool m_enable_debug_messeger;
    VkDebugUtilsMessengerEXT m_debug_messenger;

public:

    Instance(VkInstance instance, bool enable_debug_messeger = false, VkDebugUtilsMessengerEXT debug_messenger = nullptr);

    Instance(const Instance&) = delete;
    void operator=(const Instance&) = delete;

    ~Instance();
};

class InstanceBuilder
{
private:

    VkApplicationInfo m_app_info{};
    VkInstanceCreateInfo m_create_info{};
    std::vector<const char*> m_extensions{};
    std::vector<const char*> m_validation_layers = {};
    VkDebugUtilsMessengerCreateInfoEXT m_debug_create_info{};
    VkDebugUtilsMessengerEXT m_debug_messenger;
    bool m_enable_validation_layers = false;
    bool m_enable_debug_messeger = false;

public:

    InstanceBuilder();

    ~InstanceBuilder() = default;

    // Setters
    InstanceBuilder& set_debug_messeger();

    InstanceBuilder& enable_extensions(const std::vector<const char*>& extensions);

    InstanceBuilder& enable_validation_layers();

    // Build
    Instance build();
};

} // namespace kzn

#endif // INSTANCE