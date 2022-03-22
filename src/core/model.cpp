#include "core/model.hpp"

#include "core/context.hpp"
#include "core/log.hpp"

#include <optional>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <concepts>

namespace std
{
    // Template specialization of Vertex for std::hash
    template<>
    struct hash<kzn::Vertex>
    {
        size_t operator()(const kzn::Vertex& vtx) const
        {
            return kzn::multiple_hash(
                0, 
                vtx.position,
                vtx.color,
                vtx.normal,
                vtx.tex_coords);
        }
    };
} // namespace std

namespace kzn
{
    Model::Model(std::vector<Vertex>&& _vertices)
        : vertices(std::move(_vertices))
        , vbo(&Context::device(), vertices.size() * sizeof(Vertex))
        , ibo(std::nullopt)
    {
        Log::info("Vertex count: {}", vertices.size());
        vbo.upload(reinterpret_cast<float*>(vertices.data()));
    }

    Model::Model(std::vector<Vertex>&& _vertices, std::vector<uint32_t>&& _indices)
        : vertices(std::move(_vertices))
        , indices(std::move(_indices))
        , vbo(&Context::device(), vertices.size() * sizeof(Vertex))
        , ibo(std::in_place, &Context::device(), indices.size() * sizeof(uint32_t))
    {
        Log::info("Vertex count: {}", vertices.size());
        Log::info("Index count: {}", indices.size());
        vbo.upload(reinterpret_cast<float*>(vertices.data()));
        ibo.value().upload(reinterpret_cast<uint32_t*>(indices.data()));
    }

    Model Model::with_computed_indices(const std::vector<Vertex>& in_vertices)
    {
        std::vector<Vertex> computed_vertices;
        std::vector<uint32_t> computed_indices;
        computed_indices.reserve(in_vertices.size());
        
        std::unordered_map<Vertex, uint32_t> unique_vertices{};
        for(const auto& vtx : in_vertices)
        {
            if(unique_vertices.count(vtx) == 0)
            {
                unique_vertices[vtx] = static_cast<uint32_t>(unique_vertices.size());
                computed_vertices.push_back(vtx);
            }
            computed_indices.push_back(unique_vertices[vtx]);
        }

        return Model(std::move(computed_vertices), std::move(computed_indices));
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

                vertices.push_back(std::move(vtx));
            }
        }

        Log::info("Loaded model '{}'", file_path.data());
        // std::cout << "indices count: " << indices.size() << "\n";
        // std::cout << "total: " << vertices.size() * sizeof(Vertex) + indices.size() * sizeof(uint32_t) << " bytes\n";

        return Model::with_computed_indices(std::move(vertices));
    }
} // namespace kzn