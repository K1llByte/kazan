#include "mesh.hpp"

#include "graphics/scene3d.hpp"
#include "core/log.hpp"
#include "resources/resources.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/glm_element_traits.hpp>

#include <memory>
#include <vector>

namespace kzn {

MeshComponent::MeshComponent(vk::Device &device, const MeshData& mesh_data)
    : m_mesh(device, mesh_data)
    , m_material_opt{
        [&mesh_data, &device]() -> std::optional<Material3D> {
            if(mesh_data.albedo_opt.has_value()) {
                return Material3D{
                    .albedo_image = vk::Image(
                        device,
                        VkExtent3D{
                            mesh_data.albedo_opt->extent.x,
                            mesh_data.albedo_opt->extent.y,
                            mesh_data.albedo_opt->extent.z,
                        }
                    ),
                    .dset = vk::DescriptorSet(
                        device.dset_allocator().allocate(
                            device.dset_layout_cache().layout({
                                vk::sampler_binding(0),
                            })
                        )
                    ),
                };
            }
            else {
                return std::nullopt;
            }
        }()
    }
{
    if(m_material_opt.has_value()) {
        m_material_opt->albedo_image.upload(mesh_data.albedo_opt->bytes);
        m_material_opt->dset.update({m_material_opt->albedo_image.info()});
    }
}

MeshComponent::MeshComponent(vk::Device &device, const std::string_view mesh_path)
    // : MeshComponent(device, *g_resources.load<MeshData>(mesh_path))
    : MeshComponent(device, g_resources.load<Scene3DData>(mesh_path)->meshes[0])
{

}

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
        std::vector<std::uint32_t> indices;
        for (fastgltf::Mesh& mesh : gltf.meshes) {
            for(auto& primitive : mesh.primitives) {
                // TODO: Load all meshes instead of just the first
                // TODO: Load indices
                // TODO: Load tex_coords
                // TODO: Store primitive.type
                
                // Load indices
                if (primitive.indicesAccessor.has_value()) {
                    auto& accessor = gltf.accessors[primitive.indicesAccessor.value()];
                    indices.resize(accessor.count);
                    fastgltf::iterateAccessorWithIndex<uint32_t>(
                        gltf,
                        accessor,
                        [&indices](uint32_t i, size_t idx) {
                            indices[idx] = i;
                        }
                    );
                }

                // Load vertex positions
                const auto& pos_accessor = gltf.accessors[primitive.findAttribute("POSITION")->accessorIndex];
                vertices.resize(pos_accessor.count);
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf,
                    pos_accessor,
                    [&vertices](glm::vec3 pos, size_t idx) {
                        vertices[idx].position = pos;
                    }
                );

                // Load vertex normals
                auto normals = primitive.findAttribute("NORMAL");
                if (normals != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(
                        gltf,
                        gltf.accessors[normals->accessorIndex],
                        [&vertices](glm::vec3 normal, size_t idx) {
                            vertices[idx].normal = normal;
                        }
                    );
                }

                // Load vertex texture coordinates
                auto tex_coords = primitive.findAttribute("TEXCOORD_0");
                if (tex_coords != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(
                        gltf,
                        gltf.accessors[tex_coords->accessorIndex],
                        [&vertices](glm::vec2 uv, size_t idx) {
                            vertices[idx].uv = uv;
                        }
                    );
                }

                return std::make_shared<MeshData>(std::move(vertices), std::move(indices));
            }
        }
    }
    else {
        Log::error("Unsupported mesh format");
    }

    return nullptr;
}

Mesh::Mesh(
    vk::Device& device,
    const MeshData& mesh_data
)
    : m_vtx_count{mesh_data.vertices.size()}
    , m_idx_count{mesh_data.indices.size()}
    , m_vtx_buffer(device, sizeof(Vertex3D) * m_vtx_count)
    , m_idx_buffer(device, sizeof(std::uint32_t) * m_idx_count)
{
    m_vtx_buffer.upload(static_cast<const void*>(mesh_data.vertices.data()));
    m_idx_buffer.upload(mesh_data.indices.data());
}

} // namespace kzn