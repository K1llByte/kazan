#include "vk/cmd_buffers.hpp"

#include "vk/utils.hpp"

namespace kzn::vk
{
    CommandPool::CommandPool(Device* device)
        : device(device)
    {
        VkCommandPoolCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command
        // buffers are rerecorded with new commands very often (may
        // change memory allocation behavior)
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow
        // command buffers to be rerecorded individually, without
        // this flag they all have to be reset together
        create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        create_info.queueFamilyIndex = device->queue_families().graphics_family.value();
        auto result = vkCreateCommandPool(device->vk_device(), &create_info, nullptr, &vkcommand_pool);
        VK_CHECK_MSG(result, "Failed to create command pool!");
        Log::debug("Command Pool created");
    }
    
    CommandPool::~CommandPool()
    {
        // Destroy Command Pool
        vkDestroyCommandPool(device->vk_device(), vkcommand_pool, nullptr);
        Log::debug("Command Pool destroyed");
    }

    CommandBuffer CommandPool::allocate()
    {
        CommandBuffer cmd_buffer;
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = vkcommand_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;
        auto result = vkAllocateCommandBuffers(device->vk_device(), &alloc_info, &cmd_buffer.vkcommand_buffer);
        VK_CHECK_MSG(result, "Failed to allocate command buffers!");
        return cmd_buffer;
    }

    void CommandBuffer::begin(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer
        // will be rerecorded right after executing it once.
        // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary
        // command buffer that will be entirely within a single render pass.
        // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can
        // be resubmitted while it is also already pending execution.
        begin_info.flags = flags;
        begin_info.pInheritanceInfo = nullptr; // Optional
        auto result = vkBeginCommandBuffer(vkcommand_buffer, &begin_info);
        VK_CHECK_MSG(result, "Failed to begin recording command buffer!")
    }

    void CommandBuffer::end()
    {
        auto result = vkEndCommandBuffer(vkcommand_buffer);
        VK_CHECK_MSG(result, "Failed to record command buffer!")
    }

    void CommandBuffer::reset() noexcept
    {
        vkResetCommandBuffer(vkcommand_buffer, 0);
    }
} // namespace kzn::vk