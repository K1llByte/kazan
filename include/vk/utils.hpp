// TODO: Change this to KZN_VK_UTILS_HPP or equivalent
#ifndef KZN_VK_UTILS_HPP
#define KZN_VK_UTILS_HPP

#include "core/log.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

// TODO: Enhace the VkResult conversion to string
#define VK_CHECK(res)                         \
        {                                     \
            if(res != VK_SUCCESS)             \
            {                                 \
                Log::error("(VkResult = {})", \
                    res);                     \
                throw ResultError(res);       \
            }                                 \
        }

#define VK_CHECK_MSG(res, msg, ...)           \
        {                                     \
            if(res != VK_SUCCESS)             \
            {                                 \
                Log::error(msg __VA_OPT__(,)  \
                     __VA_ARGS__);            \
                throw ResultError(res);       \
            }                                 \
        }

namespace kzn::vk
{
    // FIXME: This class isn't an exception,
    // maybe consider extending std::exception
    // or other.
    class ResultError
    {
    public:
        ResultError(VkResult res)
            : result(res) {}
        ~ResultError() = default;

        VkResult raw() const { return result; }

    private:
        VkResult result;
    };
}

#endif // KZN_VK_UTILS_HPP