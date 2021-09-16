#ifndef MODEL_H
#define MODEL_H

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace kzn
{

class Model
{
public:

    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription>   bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        // VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;

        static VertexInputDescription get_vertex_description();
    };

private:

    Device&        _device;
    VkBuffer       _vertex_buffer;
    VkDeviceMemory _vertex_buffer_memory;
    uint32_t       _vertex_count;

    VkBuffer       _index_buffer;
    VkDeviceMemory _index_buffer_memory;
    uint32_t       _index_count;

public:

    Model(
        Device& device,
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices = std::vector<uint32_t>());
    ~Model();

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer);

private:

    void create_vertex_buffers(const std::vector<Vertex>& vertices);
    void create_index_buffers(const std::vector<uint32_t>& indices);

    constexpr bool has_index_buffer()
    {
        return _index_count > 0;
    }

};

}

#endif // MODEL_H
