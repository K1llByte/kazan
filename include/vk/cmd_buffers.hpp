#ifndef KZN_VK_DEVICE_HPP
#define KZN_VK_DEVICE_HPP

#include "vk/device.hpp"

namespace kzn::vk
{
    class CommandPool
    {
    public:
        CommandPool(Device* device);
        ~CommandPool();

        VkCommandPool vk_command_pool() noexcept { return vkcommand_pool; }

        CommandBuffer allocate();

    private:
        Device* device;
        VkCommandPool vkcommand_pool;
    };

    class CommandBuffer
    {
    public:
        CommandBuffer(const CommandPool& cmd_pool);
        ~CommandBuffer() = default;

        VkCommandBuffer vk_command_buffer() noexcept { return vkcommand_buffer; }

        void begin() /* const noexcept */;
        void end() /* const noexcept */;

    private:
        VkCommandBuffer vkcommand_buffer;
    };
} // namespace kzn::vk

#endif // KZN_VK_DEVICE_HPP