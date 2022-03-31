#ifndef KZN_VK_CMD_BUFFERS_HPP
#define KZN_VK_CMD_BUFFERS_HPP

#include "vk/device.hpp"

namespace kzn::vk
{
    class CommandBuffer;

    class CommandPool
    {
    public:
        CommandPool(Device* device);
        ~CommandPool();

        VkCommandPool vk_command_pool() noexcept { return vkcommand_pool; }

        CommandBuffer allocate();

    private:
        Device*       device;
        VkCommandPool vkcommand_pool;
    };

    class CommandBuffer
    {
        friend class CommandPool;
    public:
        ~CommandBuffer() = default;

        VkCommandBuffer vk_command_buffer() noexcept { return vkcommand_buffer; }

        void begin() /* const noexcept */;
        void end() /* const noexcept */;
        void reset() noexcept;

    private:
        CommandBuffer() = default;
        
    private:
        VkCommandBuffer vkcommand_buffer;
    };
} // namespace kzn::vk

#endif // KZN_VK_CMD_BUFFERS_HPP