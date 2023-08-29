#include "buffer.hpp"

#include "utils.hpp"

#include <cstring>

namespace kzn::vk {

VertexBuffer::VertexBuffer(Device& device, VkDeviceSize buffer_size)
    : m_device(device), m_buffer_size(buffer_size)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = m_buffer_size;
    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vma_alloc_info{};
    vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    auto result = vmaCreateBuffer(
        m_device.allocator(),
        &buffer_info,
        &vma_alloc_info,
        &m_buffer,
        &m_allocation,
        nullptr
    );
    VK_CHECK_MSG(result, "Failed to create Vertex Buffer");
}



VertexBuffer::~VertexBuffer() {
    vmaDestroyBuffer(m_device.allocator(), m_buffer, m_allocation);
}


void VertexBuffer::upload(const float* vertices) {
    // Copy vertex data to GPU
    void* data;
    vmaMapMemory(m_device.allocator(), m_allocation, &data);
    memcpy(data, vertices, m_buffer_size);
    vmaUnmapMemory(m_device.allocator(), m_allocation);
}


void VertexBuffer::bind(CommandBuffer& cmd_buffer) {
    // Bind Vertex Buffer
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd_buffer.vk_cmd_buffer(), 0, 1, &m_buffer, &offset);
}


IndexBuffer::IndexBuffer(Device& device, VkDeviceSize buffer_size)
    : m_device(device), m_buffer_size(buffer_size)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = m_buffer_size;
    buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    VmaAllocationCreateInfo vma_alloc_info{};
    vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    auto result = vmaCreateBuffer(
        m_device.allocator(),
        &buffer_info,
        &vma_alloc_info,
        &m_buffer,
        &m_allocation,
        nullptr
    );
    VK_CHECK_MSG(result, "Failed to create Index Buffer");
}

IndexBuffer::~IndexBuffer()
{
    vmaDestroyBuffer(m_device.allocator(), m_buffer, m_allocation);
}

void IndexBuffer::upload(const uint32_t* indices)
{
    // Copy vertex data to GPU
    void* data;
    vmaMapMemory(m_device.allocator(), m_allocation, &data);
    memcpy(data, indices, m_buffer_size);
    vmaUnmapMemory(m_device.allocator(), m_allocation);
}

void IndexBuffer::bind(CommandBuffer& cmd_buffer)
{
    // Bind Index Buffer
    vkCmdBindIndexBuffer(cmd_buffer.vk_cmd_buffer(), m_buffer, 0, VK_INDEX_TYPE_UINT32);
}

} // namespace kzn::vk
