#include "core/model.hpp"

#include "core/context.hpp"

namespace kzn
{
    Model::Model(std::vector<Vertex>&& _vertices)
        : vertices(std::move(_vertices)), vbo(&Context::device(), vertices.size() * sizeof(Vertex))
    {
        vbo.upload(reinterpret_cast<float*>(vertices.data()));
    }

    void Model::draw(vk::CommandBuffer& cmd_buffer)
    {
        vbo.bind(cmd_buffer);
        vkCmdDraw(cmd_buffer.vk_command_buffer(), vertices.size(), 1, 0, 0);
    }
} // namespace kzn