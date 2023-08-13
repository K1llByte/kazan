#include "cmd_buffer.hpp"

#include "vk/error.hpp"

#include <cassert>

namespace kzn::vk {

CommandBuffer::CommandBuffer(Device& device, VkCommandPool pool) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    auto result = vkAllocateCommandBuffers(device.vk_device(), &alloc_info, &m_vk_cmd_buffer);
    VK_CHECK_MSG(result, "Failed to allocate command buffer!");
}


void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0; // Optional
    begin_info.pInheritanceInfo = nullptr; // Optional

    auto result = vkBeginCommandBuffer(m_vk_cmd_buffer, &begin_info);
    VK_CHECK_MSG(result, "Failed to begin recording command buffer!");
}

void CommandBuffer::end() {
    auto result = vkEndCommandBuffer(m_vk_cmd_buffer);
    VK_CHECK_MSG(result, "Failed to end recording command buffer!");
}

void CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    auto result = vkResetCommandBuffer(m_vk_cmd_buffer, flags);
    VK_CHECK_MSG(result, "Failed to reset command buffer!");
}



CommandPool::CommandPool(Device& device)
    : m_device{device}
{
    assert(device.queue_families().graphics_family.has_value());
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = device.queue_families().graphics_family.value();

    auto result = vkCreateCommandPool(m_device.vk_device(), &pool_info, nullptr, &m_vk_cmd_pool);
    VK_CHECK_MSG(result, "Failed to create command pool!");
}


CommandPool::~CommandPool() {
    vkDestroyCommandPool(m_device.vk_device(), m_vk_cmd_pool, nullptr);
}


CommandBuffer CommandPool::allocate() {
    return CommandBuffer(m_device, m_vk_cmd_pool);
}

} // namespace kzn::vk
