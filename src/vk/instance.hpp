#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace kzn::vk {

struct InstanceParams {
    std::vector<char const*> extensions = {};
    std::vector<char const*> layers = {};
    bool with_validation = false;
};

class Instance {
  public:
    // Ctor
    Instance(InstanceParams&& params = {});
    // Copy
    Instance(Instance const&) = delete;
    Instance& operator=(Instance const&) = delete;
    // Move
    Instance(Instance&&) = delete;
    Instance& operator=(Instance&&) = delete;
    // Dtor
    ~Instance();

  public:
    VkInstance vk_instance() { return m_vk_instance; }

  private:
    VkInstance m_vk_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
};

} // namespace kzn::vk