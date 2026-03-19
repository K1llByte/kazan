#include "scene3d.hpp"
#include "graphics/texture.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <stb_image.h>

#include <cstddef>
#include <fstream>
#include <variant>

namespace kzn {

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
        auto& albedo = scene3d_ptr->meshes[i].albedo_opt;

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
                if(material.pbrData.baseColorTexture.has_value()) {
                    const auto& tex = gltf_asset.textures[
                        material.pbrData.baseColorTexture->textureIndex
                    ];
                    if (tex.imageIndex.has_value()) {
                        std::vector<std::byte> bytes;

                        fastgltf::Image* albedo_image_ptr = &gltf_asset.images[*tex.imageIndex];

                        if (std::holds_alternative<fastgltf::sources::URI>(albedo_image_ptr->data)) {
                            const auto& uri = std::get<fastgltf::sources::URI>(albedo_image_ptr->data);

                            // TODO: Better error handling 
                            std::ifstream file{uri.uri.c_str(), std::ios::binary};
                            // bytes = std::vector<std::byte>(
                            //     std::istreambuf_iterator<char>(file),
                            //     {}
                            // );
                        }
                        else if(std::holds_alternative<fastgltf::sources::Vector>(albedo_image_ptr->data)) {
                            bytes = std::get<fastgltf::sources::Vector>(albedo_image_ptr->data).bytes;
                        }
                        else if (std::holds_alternative<fastgltf::sources::BufferView>(albedo_image_ptr->data)) {
                            const auto& view = std::get<fastgltf::sources::BufferView>(albedo_image_ptr->data);
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

                        albedo = TextureData{};
                        albedo->bytes = result_ptr;
                        albedo->extent = Vec3u{width, height, 1};
                        // Log::debug(
                        //     "Loaded albedo texture ({}, {}, {})",
                        //     albedo->extent.x,
                        //     albedo->extent.y,
                        //     albedo->extent.z
                        // );
                    }
                }
            }
        }
    }

    return scene3d_ptr;
}

} // namespace kzn