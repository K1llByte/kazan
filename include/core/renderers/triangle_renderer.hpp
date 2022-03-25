#ifndef KZN_TRIANGLE_RENDERER_HPP
#define KZN_TRIANGLE_RENDERER_HPP

#include "core/renderer.hpp"
#include "vk/pipeline.hpp"

namespace kzn
{
    class TriangleRenderer
    {
    public:
        TriangleRenderer(Renderer* _renderer);
        ~TriangleRenderer() = default;

        void bind(vk::CommandBuffer& cmd_buffer);
        void unbind(vk::CommandBuffer& cmd_buffer);
        void draw(vk::CommandBuffer& cmd_buffer);

    private:
        Renderer*      renderer;
        vk::RenderPass render_pass;
        vk::Pipeline   pipeline;
    };
} // namespace kzn

#endif // KZN_TRIANGLE_RENDERER_HPP