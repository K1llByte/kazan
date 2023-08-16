#pragma once

#include "device.hpp"

namespace kzn::vk {

class CommandBuffer {
public:
    // Ctor
    CommandBuffer(Device& device, VkCommandPool pool);
    // Copy
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    // Move
    CommandBuffer(CommandBuffer&&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) = delete;
    // Dtor
    ~CommandBuffer() = default;

    constexpr VkCommandBuffer vk_cmd_buffer() const { return m_vk_cmd_buffer; }

    void begin(VkCommandBufferUsageFlags flags = {});
    void end();
    void reset(VkCommandBufferResetFlags flags = {});

private:
    VkCommandBuffer m_vk_cmd_buffer;
};


class CommandPool {
public:
    // Ctor
    CommandPool(Device& device);
    // Copy
    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    // Move
    CommandPool(CommandPool&&) = delete;
    CommandPool& operator=(CommandPool&&) = delete;
    // Dtor
    ~CommandPool();

    constexpr VkCommandPool vk_cmd_pool() const { return m_vk_cmd_pool; }

    vk::CommandBuffer allocate();

private:
    Device&       m_device;
    VkCommandPool m_vk_cmd_pool;
};

} // namespace kzn::vk
