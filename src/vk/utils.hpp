#pragma once

#include <array>

#include <glm/glm.hpp>

#include "vk/device.hpp"
#include "vk/error.hpp"

namespace kzn::vk {

inline VkSemaphore create_semaphore(Device& device) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphore semaphore;
    auto result = vkCreateSemaphore(device.vk_device(), &semaphore_info, nullptr, &semaphore);
    VK_CHECK_MSG(result, "Failed to created synchronization semaphore!");
    return semaphore;
}

inline VkFence create_fence(Device& device, VkFenceCreateFlags flags = {}) {
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = flags;
    VkFence fence;
    auto result = vkCreateFence(device.vk_device(), &fence_info, nullptr, &fence);
    VK_CHECK_MSG(result, "Failed to created synchronization fence!");
    return fence;
}

inline void destroy_semaphore(Device& device, VkSemaphore semaphore) {
    vkDestroySemaphore(device.vk_device(), semaphore, nullptr);
}


inline void destroy_fence(Device& device, VkFence fence) {
    vkDestroyFence(device.vk_device(), fence, nullptr);
}


// template<typename T>
// constexpr VkVertexInputBindingDescription vertex_binding(
//     uint32_t          binding,
//     VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) noexcept
// {
//     return VkVertexInputBindingDescription{
//         .binding = binding,
//         .stride = sizeof(T),
//         .inputRate = rate,
//     };
// }

constexpr VkVertexInputBindingDescription vertex_binding(
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

// Usage: vtx_attributes<vec3, vec3, vec2>();
template<typename ...Ts, std::size_t N = sizeof...(Ts)>
constexpr std::array<VkVertexInputAttributeDescription, N> vertex_attributes(
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


} // namespace kzn::vk
