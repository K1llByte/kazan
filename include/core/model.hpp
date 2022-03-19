#ifndef KZN_CORE_MODEL_HPP
#define KZN_CORE_MODEL_HPP

#include "vk/input_buffers.hpp"

#include <glm/glm.hpp>

namespace kzn
{
    struct Vertex
    {
        glm::vec2 position;
        glm::vec3 color;
    };

    class Model 
    {
    public:
        Model(std::vector<Vertex>&& _vertices);
        ~Model() = default;

        void draw(vk::CommandBuffer& cmd_buffer);

    private:
        std::vector<Vertex>   vertices;
        vk::VertexBuffer      vbo;
        // vk::IndexBuffer       ibo;
    };
} // namespace kzn

#endif // KZN_CORE_MODEL_HPP