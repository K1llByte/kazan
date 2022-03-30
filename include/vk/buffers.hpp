#ifndef KZN_VK_INPUT_BUFFER_HPP
#define KZN_VK_INPUT_BUFFER_HPP

#include "vk/device.hpp"
#include "vk/cmd_buffers.hpp"

#include "vk_mem_alloc.h"

namespace kzn::vk
{

    class VertexBuffer
    {
    public:
        VertexBuffer(Device* device, VkDeviceSize buffer_size);
        ~VertexBuffer();

        void upload(const float* vertices);
        void bind(CommandBuffer& cmd_buffer);
        
    private:
        Device*       device;
        VkBuffer      buffer;
        std::size_t   buffer_size;
        VmaAllocation allocation;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(Device* device, VkDeviceSize buffer_size);
        ~IndexBuffer();

        void upload(const uint32_t* indices);
        void bind(CommandBuffer& cmd_buffer);
    
    private:
        Device*       device;
        VkBuffer      buffer;
        std::size_t   buffer_size;
        VmaAllocation allocation;
    };

    class UniformBuffer
    {
    public:
        UniformBuffer(Device* device, VkDeviceSize buffer_size);
        ~UniformBuffer();

        // TODO: Change float to T type of the uniform buffer
        void upload(const float* data);
    
    private:
        Device*       device;
        VkBuffer      buffer;
        std::size_t   buffer_size;
        VmaAllocation allocation;
    };
} // namespace kzn::vk

#endif // KZN_VK_INPUT_BUFFER_HPP