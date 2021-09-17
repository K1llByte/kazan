#include "model.hpp"

#include "utils.hpp"

#include <stdexcept>
#include <cstring>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// Debug
#include <iostream>

namespace std 
{
    // Template specialization of Vertex for std::hash
    template<>
    struct hash<kzn::Model::Vertex>
    {
        size_t operator()(const kzn::Model::Vertex& vtx) const
        {
            return kzn::utils::multiple_hash(
                0, 
                vtx.position,
                vtx.color,
                vtx.normal,
                vtx.uv);
        }
    };

} // namespace std

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


Model* Model::load_from_file(Device& device, const std::string& file_path)
{
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    const bool compute_indices = true;
    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for(const auto& shape : shapes)
    {
        for(const auto& index : shape.mesh.indices)
        {
            Vertex vtx{};
            if(index.vertex_index >= 0)
            {
                vtx.position = {
                    attrib.vertices[3 * index.vertex_index],     // x
                    attrib.vertices[3 * index.vertex_index + 1], // y
                    attrib.vertices[3 * index.vertex_index + 2], // z
                };

                size_t color_index = 3 * index.vertex_index + 2;
                if(color_index < attrib.colors.size())
                {
                    vtx.color = {
                        attrib.colors[color_index - 2], // r
                        attrib.colors[color_index - 1], // g
                        attrib.colors[color_index]      // b
                    };
                }
                else
                {
                    // Default color
                    vtx.color = { 6.f, 6.f, 6.f };
                }
            }

            if(index.normal_index >= 0)
            {
                vtx.normal = {
                    attrib.normals[3 * index.normal_index],     // x
                    attrib.normals[3 * index.normal_index + 1], // y
                    attrib.normals[3 * index.normal_index + 2], // z
                };
            }

            if(index.texcoord_index >= 0)
            {
                vtx.uv = {
                    attrib.texcoords[3 * index.texcoord_index],     // u
                    attrib.texcoords[3 * index.texcoord_index + 1], // v
                };
            }

            if(compute_indices)
            {
                if(unique_vertices.count(vtx) == 0)
                {
                    unique_vertices[vtx] = static_cast<uint32_t>(unique_vertices.size());
                    vertices.push_back(vtx);
                }
                indices.push_back(unique_vertices[vtx]);
            }
            else
            {
                vertices.push_back(std::move(vtx));
            }
        }
    }

    std::cout << "vertex count: " << vertices.size() << "\n";
    std::cout << "indices count: " << indices.size() << "\n";
    std::cout << "total: " << vertices.size() * sizeof(Vertex) + indices.size() * sizeof(uint32_t) << " bytes\n";

    return new Model(device, vertices, indices);
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
    VkDeviceSize buffer_size = sizeof(vertices[0]) * _vertex_count;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    _device.create_buffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory);

    // Upload vertices data to device buffer
    void* data;
    vkMapMemory(_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(_device.device(), staging_buffer_memory);

    _device.create_buffer(
        buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _vertex_buffer,
        _vertex_buffer_memory);

    _device.copy_buffer(staging_buffer, _vertex_buffer, buffer_size);

    vkDestroyBuffer(_device.device(), staging_buffer, nullptr);
    vkFreeMemory(_device.device(), staging_buffer_memory, nullptr);
}


void Model::create_index_buffers(const std::vector<uint32_t>& indices)
{
    _index_count = static_cast<uint32_t>(indices.size());

    if(!has_index_buffer())
    {
        return;
    }

    // Allocate space for buffer on device
    VkDeviceSize buffer_size = sizeof(indices[0]) * _index_count;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    _device.create_buffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory);

    // Upload indices data to device buffer
    void *data;
    vkMapMemory(_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);
    std::memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(_device.device(), staging_buffer_memory);

    _device.create_buffer(
        buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _index_buffer,
        _index_buffer_memory);

    _device.copy_buffer(staging_buffer, _index_buffer, buffer_size);

    vkDestroyBuffer(_device.device(), staging_buffer, nullptr);
    vkFreeMemory(_device.device(), staging_buffer_memory, nullptr);
}

} // namespace kzn
