#pragma once

#include "device.hpp"
#include "cmd_buffer.hpp"
#include "dset.hpp"

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


class UniformBuffer {
public:
    UniformBuffer(Device& device, VkDeviceSize buffer_size);
    ~UniformBuffer();

    template<typename T> // TODO: Is uniform trait check
    void upload(const T* data);

    DescriptorInfo info() const;

private:
    Device&       m_device;
    VkBuffer      m_buffer;
    size_t        m_buffer_size;
    VmaAllocation m_allocation;
};

} // namespace kzn::vk

//////////////////////////////////////////////////////////////
//                      Implementation                      //
//////////////////////////////////////////////////////////////

namespace kzn::vk {

template<typename T>
void UniformBuffer::upload(const T* new_data) {
    // NOTE: Can't be in template requires due to
    // pfr::for_each_field not being constexpr
    // if(glsl::is_uniform<T>()) {
        // Copy uniform data to GPU
        void* data;
        vmaMapMemory(m_device.allocator(), m_allocation, &data);
        memcpy(data, new_data, m_buffer_size);
        vmaUnmapMemory(m_device.allocator(), m_allocation);
    // }
    // else {
    //     throw NotUniform();
    // }
}

} // namespace kzn::vk
