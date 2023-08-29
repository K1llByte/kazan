#pragma once

#include "device.hpp"
#include "cmd_buffer.hpp"

#include "vk_mem_alloc.h"

namespace kzn::vk {

class VertexBuffer {
public:
    // Ctor
    VertexBuffer(Device& device, VkDeviceSize buffer_size);
    // Copy
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    // Move
    VertexBuffer(VertexBuffer&&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = delete;
    // Dtor
    ~VertexBuffer();

    void upload(const float* vertices);
    void bind(CommandBuffer& cmd_buffer);
    
private:
    Device&       m_device;
    VkBuffer      m_buffer;
    size_t        m_buffer_size;
    VmaAllocation m_allocation;
};


class IndexBuffer {
public:
    // Ctor
    IndexBuffer(Device& device, VkDeviceSize buffer_size);
    // Copy
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    // Move
    IndexBuffer(IndexBuffer&&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = delete;
    // Dtor
    ~IndexBuffer();

    void upload(const uint32_t* indices);
    void bind(CommandBuffer& cmd_buffer);
    
private:
    Device&       m_device;
    VkBuffer      m_buffer;
    size_t        m_buffer_size;
    VmaAllocation m_allocation;
};

} // namespace kzn::vk
