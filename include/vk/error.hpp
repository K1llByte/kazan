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
    KZN_VK_IMPL_ERROR(NoGPUSupport, "Failed to find GPUs with Vulkan support!")
    KZN_VK_IMPL_ERROR(NoGPUSuitable, "Failed to find a suitable GPU!")

    KZN_VK_IMPL_ERROR(FailedDebugMessenger, "Failed to set up debug messenger!")
    KZN_VK_IMPL_ERROR(FailedValidationLayers, "Validation Layers not available!")
}

#endif // KZN_VK_ERROR_HPP