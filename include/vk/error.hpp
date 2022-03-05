#ifndef KZN_VK_ERROR_HPP
#define KZN_VK_ERROR_HPP

#include <exception>

#define KZN_VK_IMPL_ERROR(name, msg)          \
    class name: public std::exception         \
    {                                         \
    public:                                   \
        name() = default;                     \
        ~name() = default;                    \
        const char* what() const noexcept     \
        { return msg; }                       \
    };

namespace kzn::vk
{
    // Instance
    KZN_VK_IMPL_ERROR(NoGPUSupport, "Failed to find GPUs with Vulkan support!")
    KZN_VK_IMPL_ERROR(NoGPUSuitable, "Failed to find a suitable GPU!")

    // Device
    KZN_VK_IMPL_ERROR(FailedDebugMessenger, "Failed to set up debug messenger!")
    KZN_VK_IMPL_ERROR(FailedValidationLayers, "Validation Layers not available!")

    // Pipeline
    KZN_VK_IMPL_ERROR(ShaderModError, "Failed to create shader module!")

    // TODO: Maybe put this into core/error.hpp
    // IO
    KZN_VK_IMPL_ERROR(FileError, "Failed to open file!")
}

#endif // KZN_VK_ERROR_HPP