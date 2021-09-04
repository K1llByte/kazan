#include "mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

namespace kzn
{

VertexInputDescription Vertex::get_vertex_description()
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


bool Mesh::load_from_obj(const char* filename)
{
    // Attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    // Shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    // Materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    // Error and warning output from the load function
    std::string warn;
    std::string err;

    // Load the OBJ file
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr);
    //make sure to output the warnings to the console, in case there are issues with the file
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }
    //if we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if (!err.empty())
    {
        std::cerr << "ERROR: " << err << std::endl;
        return false;
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {

            //hardcode loading to triangles
            int fv = 3;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Vertex position
                float vx = attrib.vertices[3 * idx.vertex_index + 0];
                float vy = attrib.vertices[3 * idx.vertex_index + 1];
                float vz = attrib.vertices[3 * idx.vertex_index + 2];
                // Vertex normal
                float nx = attrib.normals[3 * idx.normal_index + 0];
                float ny = attrib.normals[3 * idx.normal_index + 1];
                float nz = attrib.normals[3 * idx.normal_index + 2];

                // Copy it into our vertex
                Vertex new_vert;
                new_vert.position.x = vx;
                new_vert.position.y = vy;
                new_vert.position.z = vz;

                new_vert.normal.x = nx;
                new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                // We are setting the vertex color as the vertex normal. This is just for display purposes
                new_vert.color = new_vert.normal;

                _vertices.push_back(new_vert);
            }
            index_offset += fv;
        }
    }

    return true;
}

}
