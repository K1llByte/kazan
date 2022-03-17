#ifndef KZN_MODEL_RENDERER_HPP
#define KZN_MODEL_RENDERER_HPP

#include "core/renderer.hpp"
#include "vk/pipeline.hpp"

namespace kzn
{
    class ModelRenderer
    {
    public:
        ModelRenderer(Renderer* renderer);
        ~ModelRenderer();

        void draw(/* Model& model */);

    private:
        Renderer*      renderer;
        vk::RenderPass render_pass;
        vk::Pipeline   pipeline;
    };
} // namespace kzn

#endif // KZN_MODEL_RENDERER_HPPModelRenderer