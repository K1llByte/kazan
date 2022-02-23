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

    KZN_VK_IMPL_ERROR(NoGPUSuitable, "Failed to find a suitable GPU!");

    // class NoGPUSupport: public std::exception
    // {
    // public:
    //     NoGPUSupport() = default;
    //     ~NoGPUSupport() = default;

    //     const char* what() const noexcept
    //     { return "Failed to find GPUs with Vulkan support!"; }
    // };

    // class NoGPUSuitable: public std::exception
    // {
    // public:
    //     NoGPUSuitable() = default;
    //     ~NoGPUSuitable() = default;

    //     const char* what() const noexcept
    //     { return "Failed to find GPUs with Vulkan support!"; }
    // };

}

#endif // KZN_VK_ERROR_HPP