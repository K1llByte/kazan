#ifndef MESH_H
#define MESH_H

#include "types.hpp"

#include <vector>

#include <glm/vec3.hpp>

namespace kzn
{

struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription>   bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription get_vertex_description();
};

struct Mesh
{
    std::vector<Vertex> _vertices;
    AllocatedBuffer     _vertex_buffer;

    bool load_from_obj(const char* filename);
};

}

#endif // MESH_H
