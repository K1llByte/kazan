#ifndef KZN_VK_UTILS_HPP
#define KZN_VK_UTILS_HPP

#include "core/log.hpp"
#include "vk/error.hpp"

// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <boost/pfr.hpp>

#include <type_traits>
#include <array>

// NOTE: If this file won't have more things besides VK_CHECK's
// then consider moving everything from error.hpp to here

namespace kzn::vk
{
    class Device;

    // TODO: Consider moving this to device method
    // or wrapper handler with destructor
    VkSemaphore create_semaphore(Device& device);
    // TODO: Consider moving this to device method
    // or wrapper handler with destructor
    // NOTE: Fence starts signaled
    VkFence create_fence(Device& device, bool signaled = true);
    void destroy_semaphore(Device& device, VkSemaphore semaphore) noexcept;
    void destroy_fence(Device& device, VkFence fence) noexcept;

    VkViewport create_viewport(VkExtent2D extent);
    VkRect2D create_scissor(VkExtent2D extent);

    // Create buffer
    // void create_buffer(Device& device, );

    // Vertex Input utilities
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
    };

    template<typename T>
    constexpr VkVertexInputBindingDescription vtx_binding(
        uint32_t          binding,
        VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) noexcept
    {
        return VkVertexInputBindingDescription{
            .binding = binding,
            .stride = sizeof(T),
            .inputRate = rate,
        };
    }

    constexpr VkVertexInputBindingDescription vtx_binding(
        uint32_t          stride,
        uint32_t          binding,
        VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) noexcept
    {
        return VkVertexInputBindingDescription{
            .binding = binding,
            .stride = stride,
            .inputRate = rate,
        };
    }

    // Runtime vertex attribute description
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

    // Usage: vtx_attributes<vec3, vec3, vec2>();
    template<typename ...Ts, std::size_t N = sizeof...(Ts)>
    constexpr std::array<VkVertexInputAttributeDescription, N> vtx_attributes(
        uint32_t binding = 0)
    {
        uint32_t location = 0;
        uint32_t offset = 0;
        std::array<VkVertexInputAttributeDescription, N> attributes{};
        auto foo = [&]<typename T>() -> void
        {
            VkFormat format;
            if(std::is_same_v<T, float>)
                format = VK_FORMAT_R32_SFLOAT;
            else if(std::is_same_v<T, glm::vec2>)
                format = VK_FORMAT_R32G32_SFLOAT;
            else if(std::is_same_v<T, glm::vec3>)
                format = VK_FORMAT_R32G32B32_SFLOAT;
            else if(std::is_same_v<T, glm::vec4>)
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
            else if(std::is_same_v<T, glm::ivec2>)
                format = VK_FORMAT_R64_SFLOAT;
            else
                throw "Invalid vertex input attribute";

            attributes[location] =
                VkVertexInputAttributeDescription{
                    .location = location,
                    .binding = binding,
                    .format = format,
                    .offset = offset,
                };
            ++location;
            offset += sizeof(T);
        };
        if constexpr (N > 0)
        {
            (foo.template operator()<Ts>(), ...);
        }

        return attributes;
    }
}

#endif // KZN_VK_UTILS_HPP