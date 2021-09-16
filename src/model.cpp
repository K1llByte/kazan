#include "model.hpp"

#include <stdexcept>
#include <cstring>

// Debug
#include <iostream>

namespace kzn
{

// Can be constexpr
Model::VertexInputDescription Model::Vertex::get_vertex_description()
{
    VertexInputDescription description;

    // We will have just 1 vertex buffer binding, with a per-vertex rate
    VkVertexInputBindingDescription main_binding{};
    main_binding.binding = 0;
    main_binding.stride = sizeof(Vertex);
    main_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    description.bindings.push_back(main_binding);

    // Position will be stored at Location 0
    VkVertexInputAttributeDescription position_attribute{};
    position_attribute.binding = 0;
    position_attribute.location = 0;
    position_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    position_attribute.offset = offsetof(Vertex, position);

     // Normal will be stored at Location 1
    VkVertexInputAttributeDescription normal_attribute{};
    normal_attribute.binding = 0;
    normal_attribute.location = 1;
    normal_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normal_attribute.offset = offsetof(Vertex, normal);

    // Color will be stored at Location 2
    VkVertexInputAttributeDescription color_attribute{};
    color_attribute.binding = 0;
    color_attribute.location = 2;
    color_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    color_attribute.offset = offsetof(Vertex, color);

    description.attributes.push_back(position_attribute);
    description.attributes.push_back(normal_attribute);
    description.attributes.push_back(color_attribute);
    return description;
}


Model::Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : _device{device}
{
    create_vertex_buffers(vertices);
    create_index_buffers(indices);
}


Model::~Model()
{
    vkDestroyBuffer(_device.device(), _vertex_buffer, nullptr);
    vkFreeMemory(_device.device(), _vertex_buffer_memory, nullptr);

    if(has_index_buffer())
    {
        vkDestroyBuffer(_device.device(), _index_buffer, nullptr);
        vkFreeMemory(_device.device(), _index_buffer_memory, nullptr);
    }
}


void Model::bind(VkCommandBuffer command_buffer)
{
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer, offsets);

    if(has_index_buffer())
    {
        vkCmdBindIndexBuffer(command_buffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}


void Model::draw(VkCommandBuffer command_buffer)
{
    if(has_index_buffer())
    {
        vkCmdDrawIndexed(command_buffer, _index_count, 1, 0, 0, 0);
    }
    else
    {
        vkCmdDraw(command_buffer, _vertex_count, 1, 0, 0);
    }
}


void Model::create_vertex_buffers(const std::vector<Vertex>& vertices)
{
    _vertex_count = static_cast<uint32_t>(vertices.size());

    if(_vertex_count < 3)
    {
        throw std::runtime_error("Vertex count must be at least 3");
    }

    // Allocate space for buffer on device
    VkDeviceSize _buffer_size = sizeof(vertices[0]) * _vertex_count;
    _device.create_buffer(
        _buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        _vertex_buffer,
        _vertex_buffer_memory);

    // Upload vertices data to device buffer
    void *data;
    vkMapMemory(_device.device(), _vertex_buffer_memory, 0, _buffer_size, 0, &data);
    std::memcpy(data, vertices.data(), static_cast<size_t>(_buffer_size));
    vkUnmapMemory(_device.device(), _vertex_buffer_memory);
}


void Model::create_index_buffers(const std::vector<uint32_t>& indices)
{
    _index_count = static_cast<uint32_t>(indices.size());

    if(!has_index_buffer())
    {
        return;
    }

    // Allocate space for buffer on device
    VkDeviceSize _buffer_size = sizeof(indices[0]) * _index_count;
    _device.create_buffer(
        _buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        _index_buffer,
        _index_buffer_memory);

    // Upload vertices data to device buffer
    void *data;
    vkMapMemory(_device.device(), _index_buffer_memory, 0, _buffer_size, 0, &data);
    std::memcpy(data, indices.data(), static_cast<size_t>(_buffer_size));
    vkUnmapMemory(_device.device(), _index_buffer_memory);
}

}
