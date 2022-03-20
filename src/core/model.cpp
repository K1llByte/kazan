#include "core/model.hpp"

#include "core/context.hpp"
#include "core/log.hpp"

#include <optional>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace kzn
{
    Model::Model(std::vector<Vertex>&& _vertices)
        : vertices(std::move(_vertices))
        , vbo(&Context::device(), vertices.size() * sizeof(Vertex))
        , ibo(std::nullopt)
    {
        vbo.upload(reinterpret_cast<float*>(vertices.data()));
    }

    Model::Model(std::vector<Vertex>&& _vertices, std::vector<uint32_t>&& _indices)
        : vertices(std::move(_vertices))
        , indices(std::move(_indices))
        , vbo(&Context::device(), vertices.size() * sizeof(Vertex))
        , ibo(std::in_place, &Context::device(), indices.size() * sizeof(uint32_t))
    {
        vbo.upload(reinterpret_cast<float*>(vertices.data()));
        ibo.value().upload(reinterpret_cast<uint32_t*>(indices.data()));
    }
    
    void Model::draw(vk::CommandBuffer& cmd_buffer)
    {
        vbo.bind(cmd_buffer);
        if(indices.empty())
        {
            vkCmdDraw(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0);
        }
        else
        {
            ibo.value().bind(cmd_buffer);
            vkCmdDrawIndexed(cmd_buffer.vk_command_buffer(), indices.size(), 1, 0, 0, 0);
        }
    }

    Model Model::load(const std::string_view& file_path)
    {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.data()))
        {
            // TODO: Change to custom kazan exception
            throw std::runtime_error(warn + err);
        }

        // const bool compute_indices = true;
        // std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for(const auto& shape : shapes)
        {
            for(const auto& index : shape.mesh.indices)
            {
                Vertex vtx{};
                if(index.vertex_index >= 0)
                {
                    vtx.position = {
                        attrib.vertices[3 * index.vertex_index + 2], // z
                        attrib.vertices[3 * index.vertex_index + 1], // y
                        attrib.vertices[3 * index.vertex_index],     // x
                    };

                    vtx.color = {
                        attrib.colors[3 * index.vertex_index],     // x
                        attrib.colors[3 * index.vertex_index + 1], // y
                        attrib.colors[3 * index.vertex_index + 2], // z
                    };
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
                    vtx.tex_coords = {
                        attrib.texcoords[2 * index.texcoord_index],     // u
                        1. - attrib.texcoords[2 * index.texcoord_index + 1], // v
                    };
                    
                }

                // if(compute_indices)
                // {
                //     if(unique_vertices.count(vtx) == 0)
                //     {
                //         unique_vertices[vtx] = static_cast<uint32_t>(unique_vertices.size());
                //         vertices.push_back(vtx);
                //     }
                //     indices.push_back(unique_vertices[vtx]);
                // }
                // else
                // {
                vertices.push_back(std::move(vtx));
                // }
            }
        }

        Log::info("Loaded model '{}'", file_path.data());
        Log::info("Vertex count: {}", vertices.size());
        // std::cout << "indices count: " << indices.size() << "\n";
        // std::cout << "total: " << vertices.size() * sizeof(Vertex) + indices.size() * sizeof(uint32_t) << " bytes\n";

        return Model(std::move(vertices)); // , std::move(indices)
    }
} // namespace kzn