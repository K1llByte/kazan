#include "instance.hpp"

#include <stdexcept>
#include <cstring>

#include <iostream>

namespace kzn
{

InstanceBuilder::InstanceBuilder()
{
    m_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    m_app_info.pApplicationName = "Hello Vulkan";
    m_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    m_app_info.pEngineName = "Kazan";
    m_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    m_app_info.apiVersion = VK_API_VERSION_1_0;

    m_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    m_create_info.pApplicationInfo = &m_app_info;

    // By default validation layers won't come
    // enabled
    m_create_info.enabledLayerCount = 0;
    m_create_info.pNext = nullptr;

    // // List supported extensions
    // uint32_t extension_count = 0;
    // vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    // std::vector<VkExtensionProperties> extensions(extension_count);
    // vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());


    // if(ENABLE_VALIDATION_LAYERS)
    // {
    //     extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    // }
}

InstanceBuilder& InstanceBuilder::enable_extensions(const std::vector<const char*>& extensions)
{
    m_extensions.insert(
        m_extensions.end(),
        extensions.begin(),
        extensions.end()
        );
    return *this;
}

// InstanceBuilder& InstanceBuilder::enable_extension(const char* extension)
// {
//     m_extensions.push_back(extensions);
//     return *this;
// }

bool check_validation_layers_support(const std::vector<const char*>& validation_layers)
{
    // Retrieve avaiable validation layers
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    // Check for each wanted validation layers
    // that they exist in the available layers
    // vector
    for (const char* layerName : validation_layers)
    {
        bool layer_found = false;

        for (const auto& layerProperties : available_layers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
            return false;
    }

    return true;
}

InstanceBuilder& InstanceBuilder::enable_validation_layers()
{
    m_validation_layers.push_back("VK_LAYER_KHRONOS_validation");

    if(!check_validation_layers_support(m_validation_layers))
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    m_create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
    m_create_info.ppEnabledLayerNames = m_validation_layers.data();

    // m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return *this;
}

Instance InstanceBuilder::build()
{
    m_create_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
    m_create_info.ppEnabledExtensionNames = m_extensions.data();

    ///////////////// LOGS /////////////////
    std::cout << "[INFO] > Enabled Extensions:\n";
    for (const auto& ext : m_extensions)
        std::cout << " - " << ext << '\n';
    ////////////////////////////////////////

    Instance instance;

    // Create instance
    if(vkCreateInstance(&m_create_info, nullptr, &instance.m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    return instance;
}

} // namespace kzn