#include "mesh.hpp"

#include "core/log.hpp"
#include "resources/resource.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include <fastgltf/glm_element_traits.hpp>

#include <memory>
#include <vector>

namespace kzn {

std::shared_ptr<MeshData> MeshData::load(const std::filesystem::path& path) {
    auto& path_str = path.native();
    if(path_str.ends_with(".gltf") || path_str.ends_with(".glb")) {
        
        // Map gltf file
        auto gltf_file_res = fastgltf::MappedGltfFile::FromPath(path);
        if (gltf_file_res.error() != fastgltf::Error::None) {
            throw LoadingError{
                .message = fastgltf::getErrorMessage(gltf_file_res.error()).data(),
            };
		}
        
        // Load gltf model
        fastgltf::Parser parser;
        constexpr auto gltf_options = fastgltf::Options::LoadExternalBuffers;
        auto asset_res = parser.loadGltf(gltf_file_res.get(), path.parent_path(), gltf_options);
        if (asset_res.error() != fastgltf::Error::None) {
            throw LoadingError{
                .message = fastgltf::getErrorMessage(asset_res.error()).data(),
            };
        }

        auto& gltf = asset_res.get();
        
        std::vector<Vertex3D> vertices;
        for (fastgltf::Mesh& mesh : gltf.meshes) {
            for(auto& primitive : mesh.primitives) {
                // TODO: Load all meshes instead of just the first
                // TODO: Load indices
                // TODO: Load normals
                // TODO: Load tex_coords
                // TODO: Store primitive.type

                // Load vertex positions
                const auto& pos_accessor = gltf.accessors[primitive.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + pos_accessor.count);
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf,
                    pos_accessor,
                    [&vertices](glm::vec3 pos, size_t idx) {
                        vertices[idx].position = pos;
                    }
                );

                return std::make_shared<MeshData>(std::move(vertices));
            }
        }
    }
    else {
        Log::error("Unsupported mesh format");
    }

    return nullptr;
}

Mesh::Mesh(vk::Device& device, const std::vector<Vertex3D>& vertices)
    : m_vtx_count{vertices.size()}
    , m_vtx_buffer(device, sizeof(Vertex3D) * m_vtx_count)
{
    m_vtx_buffer.upload(static_cast<const void*>(vertices.data()));
}

} // namespace kzn