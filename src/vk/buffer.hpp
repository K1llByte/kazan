#pragma once

#include "cmd_buffer.hpp"
#include "core/assert.hpp"
#include "device.hpp"
#include "dset.hpp"

#include "uniform.hpp"
#include "vk_mem_alloc.h"
#include <cassert>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace kzn::vk {

class VertexBuffer {
public:
    // Ctor
    VertexBuffer(Device& device, VkDeviceSize buffer_size);
    // Copy
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    // Move
    VertexBuffer(VertexBuffer&&);
    VertexBuffer& operator=(VertexBuffer&&);
    // Dtor
    ~VertexBuffer();

    void upload(const void* vertices);

    void bind(CommandBuffer& cmd_buffer);

private:
    Device* m_device_ptr;
    VkBuffer m_buffer;
    size_t m_buffer_size;
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
    IndexBuffer(IndexBuffer&&);
    IndexBuffer& operator=(IndexBuffer&&);
    // Dtor
    ~IndexBuffer();

    void upload(const uint32_t* indices);

    void bind(CommandBuffer& cmd_buffer);

    [[nodiscard]]
    uint32_t size() const {
        KZN_ASSERT(m_buffer_size % sizeof(uint32_t) == 0);
        return m_buffer_size / sizeof(uint32_t);
    }

private:
    Device* m_device_ptr;
    VkBuffer m_buffer;
    VkDeviceSize m_buffer_size;
    VmaAllocation m_allocation;
};

class UniformBuffer {
public:
    // Ctor
    UniformBuffer(Device& device, VkDeviceSize buffer_size);
    // Copy
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    // Move
    UniformBuffer(UniformBuffer&&);
    UniformBuffer& operator=(UniformBuffer&&);
    // Dtor
    ~UniformBuffer();

    void upload(const glsl::UniformBlock auto& data);

    [[nodiscard]]
    DescriptorInfo info() const;

private:
    Device* m_device_ptr;
    VkBuffer m_buffer;
    size_t m_buffer_size;
    VmaAllocation m_allocation;
};

} // namespace kzn::vk

//////////////////////////////////////////////////////////////
//                      Implementation                      //
//////////////////////////////////////////////////////////////

namespace kzn::vk {

void UniformBuffer::upload(const glsl::UniformBlock auto& new_data) {
    // Copy uniform data to GPU
    void* data;
    vmaMapMemory(m_device_ptr->allocator(), m_allocation, &data);
    memcpy(data, &new_data, m_buffer_size);
    vmaUnmapMemory(m_device_ptr->allocator(), m_allocation);
}

} // namespace kzn::vk
