#ifndef KZN_MODEL_RENDERER_HPP
#define KZN_MODEL_RENDERER_HPP

#include "core/renderer.hpp"
#include "vk/pipeline.hpp"

namespace kzn
{
    struct PVM
    {
        glm::mat4 proj_view;
        glm::mat4 model;
    };

    class ModelRenderer
    {
    public:
        ModelRenderer(Renderer* _renderer);
        ~ModelRenderer() = default;

        void bind(vk::CommandBuffer& cmd_buffer);
        void push(vk::CommandBuffer& cmd_buffer, PVM& pvm);
        void unbind(vk::CommandBuffer& cmd_buffer);
        void draw(vk::CommandBuffer& cmd_buffer);

    private:
        Renderer*      renderer;
        vk::RenderPass render_pass;
        vk::Pipeline   pipeline;
    };
} // namespace kzn

#endif // KZN_MODEL_RENDERER_HPPModelRenderer