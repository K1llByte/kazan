#ifndef KZN_CORE_MODEL_HPP
#define KZN_CORE_MODEL_HPP

#include "core/transform.hpp"
#include "vk/buffers.hpp"

// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace kzn
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 tex_coords;

        bool operator==(const Vertex& other) const
        {
            return position == other.position &&
                normal == other.normal &&
                color == other.color &&
                tex_coords == other.tex_coords;
        }
    };

    class Model 
    {
    public:
        Model(std::vector<Vertex>&& _vertices);
        Model(std::vector<Vertex>&& _vertices, std::vector<uint32_t>&& _indices);
        ~Model() = default;
        static Model with_computed_indices(const std::vector<Vertex>& in_vertices);

        static Model load(const std::string_view& file_path);

        void draw(vk::CommandBuffer& cmd_buffer);

    public:
        TransformComponent transform{};

    private:
        std::vector<Vertex>            vertices;
        std::vector<uint32_t>          indices;
        vk::VertexBuffer               vbo;
        std::optional<vk::IndexBuffer> ibo;
    };

    template<typename T>
    constexpr size_t multiple_hash(size_t seed, const T& head)
    {
        const size_t this_seed = std::hash<T>{}(head) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed ^ this_seed;
    }

    template<typename T, typename... Args>
    constexpr size_t multiple_hash(size_t seed, const T& head, const Args&... tail)
    {
        const size_t this_seed = std::hash<T>{}(head) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        const size_t tail_seed = multiple_hash(this_seed, tail...);
        return seed ^ tail_seed;
    }
} // namespace kzn

#endif // KZN_CORE_MODEL_HPP