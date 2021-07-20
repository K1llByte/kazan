#ifndef DEVICE
#define DEVICE

#include "window.hpp"

#include <stdexcept>
#include <vector>
#include <set>
#include <optional>
#include <cstdlib>
#include <cstring>

namespace kzn
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete()
    {
        return graphics_family.has_value()
            && present_family.has_value();
    }
};


class Device
{
private:
    kzn::Window& m_window;

    VkInstance m_instance;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
    VkSurfaceKHR m_surface;

    const bool enable_validation_layers = false;
    const std::vector<const char *> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};

public:
    Device(kzn::Window& window);
    ~Device();

    // Cannot be copyable or movable
    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    void run() {}

private:

    void create_instance();

    std::vector<const char *> get_required_extensions();

    bool has_required_extensions();

    bool has_validation_layers_support();

    void pick_physical_device();

    bool is_device_suitable(VkPhysicalDevice device);

    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

    void create_logical_device();

    void create_surface();
};

}

#endif // DEVICE