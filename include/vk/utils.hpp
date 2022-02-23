// TODO: Change this to KZN_VK_UTILS_HPP or equivalent
#ifndef KZN_VK_UTILS_HPP
#define KZN_VK_UTILS_HPP

#include "core/log.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

// NOTE: If this file won't have more things besides VK_CHECK's
// then consider moving everything from error.hpp to here
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
    // FIXME: Make this class extend std::exception and
    // implement what()
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