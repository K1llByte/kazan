#ifndef KZN_MODEL_RENDERER_HPP
#define KZN_MODEL_RENDERER_HPP

#include "core/renderer.hpp"
#include "vk/pipeline.hpp"

namespace kzn
{
    class ModelRenderer
    {
    public:
        ModelRenderer(Renderer* _renderer);
        ~ModelRenderer() = default;

        void bind(vk::CommandBuffer& cmd_buffer, bool render_wireframe = false);
        template<typename T>
        void push(vk::CommandBuffer& cmd_buffer, const T& data);
        void unbind(vk::CommandBuffer& cmd_buffer);
        void draw(vk::CommandBuffer& cmd_buffer);

    private:
        Renderer*      renderer;
        vk::RenderPass render_pass;
        vk::Pipeline   pipeline;
    };

    template<typename T>
    void ModelRenderer::push(vk::CommandBuffer& cmd_buffer, const T& data)
    {
        vkCmdPushConstants(
            cmd_buffer.vk_command_buffer(),
            pipeline.layout(),
            VK_SHADER_STAGE_ALL_GRAPHICS,
            0,
            sizeof(T),
            &data
        );
    }
} // namespace kzn

#endif // KZN_MODEL_RENDERER_HPP