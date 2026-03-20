#include "scene3d.hpp"
#include "graphics/material3d.hpp"
#include "graphics/texture.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <stb_image.h>

#include <cstddef>
#include <fstream>
#include <variant>

namespace kzn {

[[nodiscard]]
static inline TextureData load_image(fastgltf::Asset& gltf_asset, std::size_t image_idx) {
    std::vector<std::byte> bytes;

    fastgltf::Image* image_ptr = &gltf_asset.images[image_idx];

    if (std::holds_alternative<fastgltf::sources::URI>(image_ptr->data)) {
        const auto& uri = std::get<fastgltf::sources::URI>(image_ptr->data);

        std::ifstream file{uri.uri.c_str(), std::ios::binary};
        if(file) {
            file.seekg(0, std::ios::end);
            const auto size = static_cast<std::size_t>(file.tellg());
            file.seekg(0, std::ios::beg);

            bytes.resize(size);
            file.read(reinterpret_cast<char*>(bytes.data()), size);
        }
    }
    else if(std::holds_alternative<fastgltf::sources::Vector>(image_ptr->data)) {
        bytes = std::get<fastgltf::sources::Vector>(image_ptr->data).bytes;
    }
    else if (std::holds_alternative<fastgltf::sources::BufferView>(image_ptr->data)) {
        const auto& view = std::get<fastgltf::sources::BufferView>(image_ptr->data);
        const auto& buffer_view = gltf_asset.bufferViews[view.bufferViewIndex];
        const auto& buffer = gltf_asset.buffers[buffer_view.bufferIndex];

        const auto& data = std::get<fastgltf::sources::Array>(buffer.data);
        bytes = std::vector<std::byte>(
            data.bytes.begin() + buffer_view.byteOffset,
            data.bytes.begin() + buffer_view.byteOffset + buffer_view.byteLength
        );
    }

    // Decode image format
    int width;
    int height;
    int channels;
    unsigned char* result_ptr = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(bytes.data()),
        static_cast<int>(bytes.size()),
        &width, &height, &channels,
        STBI_rgb_alpha
    );

    if (result_ptr == nullptr) {
        throw LoadingError{stbi_failure_reason()};
    }

    return TextureData(result_ptr, Vec3u{width, height, 1});
}

std::shared_ptr<Scene3DData> Scene3DData::load(const std::filesystem::path& path) {
    auto& path_str = path.native();
    if(!path_str.ends_with(".gltf") && !path_str.ends_with(".glb")) {
        Log::error("Unsupported Scene3D format");
        return nullptr;
    }

    // Map gltf file
    auto gltf_file_res = fastgltf::MappedGltfFile::FromPath(path);
    if (gltf_file_res.error() != fastgltf::Error::None) {
        throw LoadingError{
            .message = fastgltf::getErrorMessage(gltf_file_res.error()).data(),
        };
    }

    // Load gltf model
    fastgltf::Parser parser;
    constexpr auto gltf_options = fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages;
    auto asset_res = parser.loadGltf(gltf_file_res.get(), path.parent_path(), gltf_options);
    if (asset_res.error() != fastgltf::Error::None) {
        throw LoadingError{
            .message = fastgltf::getErrorMessage(asset_res.error()).data(),
        };
    }

    auto& gltf_asset = asset_res.get();

    auto scene3d_ptr = std::make_shared<Scene3DData>();

    scene3d_ptr->meshes.resize(gltf_asset.meshes.size());
    for(std::size_t i = 0; i < gltf_asset.meshes.size(); ++i) {
        fastgltf::Mesh& gltf_mesh = gltf_asset.meshes[i];
        auto& vertices = scene3d_ptr->meshes[i].vertices;
        auto& indices = scene3d_ptr->meshes[i].indices;

        for(auto& primitive : gltf_mesh.primitives) {
            
            ///////////////////////////////////////////////////////////////////
            // Load primitive vertex data
            ///////////////////////////////////////////////////////////////////

            // TODO: Store primitive.type
            // TODO: Store attributes layout
            // TODO: Load material textures
            
            // Load indices
            if (primitive.indicesAccessor.has_value()) {
                auto& accessor = gltf_asset.accessors[primitive.indicesAccessor.value()];
                indices.resize(accessor.count);
                fastgltf::iterateAccessorWithIndex<std::uint32_t>(
                    gltf_asset,
                    accessor,
                    [&indices](std::uint32_t i, std::size_t idx) {
                        indices[idx] = i;
                    }
                );
            }

            // Load vertex positions
            const auto& pos_accessor = gltf_asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];
            vertices.resize(pos_accessor.count);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(
                gltf_asset,
                pos_accessor,
                [&vertices](glm::vec3 pos, std::size_t idx) {
                    vertices[idx].position = pos;
                }
            );

            // Load vertex normals
            auto normals = primitive.findAttribute("NORMAL");
            if (normals != primitive.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    gltf_asset,
                    gltf_asset.accessors[normals->accessorIndex],
                    [&vertices](glm::vec3 normal, std::size_t idx) {
                        vertices[idx].normal = normal;
                    }
                );
            }

            // Load vertex texture coordinates
            auto tex_coords = primitive.findAttribute("TEXCOORD_0");
            if (tex_coords != primitive.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(
                    gltf_asset,
                    gltf_asset.accessors[tex_coords->accessorIndex],
                    [&vertices](glm::vec2 uv, std::size_t idx) {
                        vertices[idx].uv = uv;
                    }
                );
            }

            ///////////////////////////////////////////////////////////////////
            // Load primitive material data
            ///////////////////////////////////////////////////////////////////
            
            if (primitive.materialIndex.has_value()) {
                const auto& material = gltf_asset.materials[*primitive.materialIndex];

                scene3d_ptr->meshes[i].material_opt = MaterialData{};
                auto& material_data = *scene3d_ptr->meshes[i].material_opt;
                
                // Load albedo image
                if(material.pbrData.baseColorTexture.has_value()) {
                    const auto& texture = gltf_asset.textures[
                        material.pbrData.baseColorTexture->textureIndex
                    ];
                    if (texture.imageIndex.has_value()) {
                        material_data.albedo_opt = load_image(gltf_asset, texture.imageIndex.value());
                    }
                }

                // Load normal map image
                if(material.normalTexture.has_value()) {
                    const auto& texture = gltf_asset.textures[
                        material.normalTexture->textureIndex
                    ];
                    if (texture.imageIndex.has_value()) {
                        material_data.normal_opt = load_image(gltf_asset, texture.imageIndex.value());
                    }
                }

                // Load metallic roughness image
                if(material.pbrData.metallicRoughnessTexture.has_value()) {
                    const auto& texture = gltf_asset.textures[
                        material.pbrData.metallicRoughnessTexture->textureIndex
                    ];
                    if (texture.imageIndex.has_value()) {
                        material_data.metallic_roughness_opt = load_image(gltf_asset, texture.imageIndex.value());
                    }
                }

                // Load occlusion map image
                if(material.occlusionTexture.has_value()) {
                    const auto& texture = gltf_asset.textures[
                        material.occlusionTexture->textureIndex
                    ];
                    if (texture.imageIndex.has_value()) {
                        material_data.occlusion_opt = load_image(gltf_asset, texture.imageIndex.value());
                    }
                }
            }
        }
    }

    return scene3d_ptr;
}

} // namespace kzn