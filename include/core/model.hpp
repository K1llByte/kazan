#ifndef KZN_CORE_MODEL_HPP
#define KZN_CORE_MODEL_HPP

#include "vk/input_buffers.hpp"

#include <glm/glm.hpp>

namespace kzn
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 tex_coords;
    };

    class Model 
    {
    public:
        Model(std::vector<Vertex>&& _vertices);
        Model(std::vector<Vertex>&& _vertices, std::vector<uint32_t>&& _indices);
        ~Model() = default;

        static Model load(const std::string_view& file_path);

        void draw(vk::CommandBuffer& cmd_buffer);

    private:
        std::vector<Vertex>            vertices;
        std::vector<uint32_t>          indices;
        vk::VertexBuffer               vbo;
        std::optional<vk::IndexBuffer> ibo;
    };
} // namespace kzn

#endif // KZN_CORE_MODEL_HPP