#ifndef INSTANCE
#define INSTANCE

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

#include <vector>
#include <memory>

namespace kzn
{

class Instance
{
public:

    struct TemporaryData
    {
        VkApplicationInfo                  _app_info;
        VkInstanceCreateInfo               _create_info;
        VkDebugUtilsMessengerCreateInfoEXT _debug_create_info;
    };

public:

    VkInstance     _instance;
    TemporaryData* _tmp_data = nullptr;
    // Optional data
    std::vector<const char*> _extensions{};
    std::vector<const char*> _validation_layers{};
    VkDebugUtilsMessengerEXT _debug_messenger;
    bool                     _enable_validation_layers = false;
    bool                     _enable_debug_messeger = false;

public:

    Instance();
    ~Instance();

    Instance& set_debug_messeger();
    Instance& enable_extensions(const std::vector<const char*>& extensions);
    Instance& enable_validation_layers();

    void init();
    void cleanup();

    // Getters
    VkInstance instance() const;
    bool initialized() const;
};

} // namespace kzn

#endif // INSTANCE