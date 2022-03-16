#ifndef KZN_VK_UTILS_HPP
#define KZN_VK_UTILS_HPP

#include "core/log.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#   include <vulkan/vulkan.h>
#endif

// NOTE: If this file won't have more things besides VK_CHECK's
// then consider moving everything from error.hpp to here

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
    // implement what() with vk spec error msg
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

    class Device;

    // TODO: Consider moving this to device method
    // or wrapper handler with destructor
    VkSemaphore create_semaphore(Device& device);
    // TODO: Consider moving this to device method
    // or wrapper handler with destructor
    // NOTE: Fence starts signaled
    VkFence create_fence(Device& device);
    void destroy_semaphore(Device& device, VkSemaphore semaphore) noexcept;
    void destroy_fence(Device& device, VkFence fence) noexcept;

    VkViewport create_viewport(VkExtent2D extent);
    VkRect2D create_scissor(VkExtent2D extent);

    // Vertex Input utilities
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
    };

    template<typename T>
    constexpr VkVertexInputBindingDescription vtx_binding(
        uint32_t binding,
        VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) noexcept
    {
        return VkVertexInputBindingDescription{
            .binding = binding,
            .stride = sizeof(T),
            .inputRate = rate,
        };
    }

    constexpr VkVertexInputAttributeDescription vtx_attribute(
        uint32_t binding,
        uint32_t location,
        VkFormat format,
        uint32_t offset) noexcept
    {
        return VkVertexInputAttributeDescription{
            .location = location,
            .binding = binding,
            .format = format,
            .offset = offset,
        };
    }
}

#endif // KZN_VK_UTILS_HPP