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

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};


class Device
{
private:
    kzn::Window& m_window;

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;
    VkSurfaceKHR m_surface;

    const bool enable_validation_layers = false;
    const std::vector<const char*> m_validation_layers = {
        "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> m_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME };

public:
    Device(kzn::Window& window);
    ~Device();

    // Cannot be copyable or movable
    Device(const Device &) = delete;
    void operator=(const Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &&) = delete;

    void run() {}

    SwapChainSupportDetails get_swap_chain_support();
    
    VkSurfaceKHR surface() { return m_surface; }
    VkDevice device() { return m_device; }

    QueueFamilyIndices get_physical_queue_families();
    
private:

    void create_instance();

    std::vector<const char *> get_required_extensions();

    bool has_required_extensions();

    bool has_validation_layers_support();

    void setup_debug_messenger();

    void pick_physical_device();

    bool is_device_suitable(VkPhysicalDevice device);

    bool has_device_extensions_support(VkPhysicalDevice device);

    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

    void create_logical_device();

    void create_surface();

    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
};

}

#endif // DEVICE