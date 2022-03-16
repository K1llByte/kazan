#include "vk/input_buffers.hpp"

#include "vk/utils.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace kzn::vk
{
    VertexBuffer::VertexBuffer(Device* _device, VkDeviceSize _buffer_size)
        : device(_device), buffer_size(_buffer_size)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = buffer_size;
        buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo vma_alloc_info{};
	    vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        auto result = vmaCreateBuffer(
            device->allocator(),
            &buffer_info,
            &vma_alloc_info,
            &buffer,
            &allocation,
            nullptr
        );
        VK_CHECK_MSG(result, "Failed to create Vertex Buffer");
    }

    VertexBuffer::~VertexBuffer()
    {
        vmaDestroyBuffer(device->allocator(), buffer, allocation);
    }

    void VertexBuffer::upload(const float* vertices)
    {
        // Copy vertex data to GPU
        void* data;
        vmaMapMemory(device->allocator(), allocation, &data);
        memcpy(data, vertices, buffer_size);
        vmaUnmapMemory(device->allocator(), allocation);
    }

    void VertexBuffer::bind(CommandBuffer& cmd_buffer)
    {
        // Bind Vertex Buffer
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd_buffer.vk_command_buffer(), 0, 1, &buffer, &offset);
    }

    IndexBuffer::IndexBuffer(Device* _device, VkDeviceSize _buffer_size)
        : device(_device), buffer_size(_buffer_size)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = buffer_size;
        buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        VmaAllocationCreateInfo vma_alloc_info{};
	    vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        auto result = vmaCreateBuffer(
            device->allocator(),
            &buffer_info,
            &vma_alloc_info,
            &buffer,
            &allocation,
            nullptr
        );
        VK_CHECK_MSG(result, "Failed to create Vertex Buffer");
    }

    IndexBuffer::~IndexBuffer()
    {
        vmaDestroyBuffer(device->allocator(), buffer, allocation);
    }

    void IndexBuffer::upload(const uint32_t* vertices)
    {
        // Copy vertex data to GPU
        void* data;
        vmaMapMemory(device->allocator(), allocation, &data);
        memcpy(data, vertices, buffer_size);
        vmaUnmapMemory(device->allocator(), allocation);
    }

    void IndexBuffer::bind(CommandBuffer& cmd_buffer)
    {
        // Bind Index Buffer
        vkCmdBindIndexBuffer(cmd_buffer.vk_command_buffer(), buffer, 0, VK_INDEX_TYPE_UINT32);
    }
} // namespace kzn::vk