#ifndef KZN_MODEL_RENDERER_HPP
#define KZN_MODEL_RENDERER_HPP

#include "core/renderer.hpp"
#include "vk/pipeline.hpp"
#include "vk/descriptor_set.hpp"
#include "vk/buffers.hpp"
#include "core/texture.hpp"
#include "vk/image.hpp"
#include "vk/framebuffer.hpp"

namespace kzn {
    struct Tmp {
        glsl::vec3 bias1 = glm::vec3{0.0, 1.0, 0.0};
        glsl::vec3 bias2 = glm::vec3{1.0, 1.0, 1.0};
    };

    struct PVM {
        glsl::mat4 proj_view;
        glsl::mat4 model;
    };

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
        Renderer*         renderer;
        vk::RenderPass    render_pass;
        vk::Framebuffers  framebuffers;

    public: // FIXME: TEMPORARY
        vk::DescriptorSetAllocator   allocator;
        vk::DescriptorSetLayoutCache cache;
        vk::UniformBuffer            ubo;
        Texture                      tex_data;
        vk::Image                    tex;
        vk::DescriptorSet            desc_set;
    
    private:
        vk::Pipeline      pipeline;
        vk::Pipeline      wireframe_pipeline;
        
    };

    template<typename T>
    void ModelRenderer::push(vk::CommandBuffer& cmd_buffer, const T& data)
    {
        if(glsl::is_uniform<T>()) {
            vkCmdPushConstants(
                cmd_buffer.vk_command_buffer(),
                pipeline.layout(),
                VK_SHADER_STAGE_ALL_GRAPHICS,
                0,
                sizeof(T),
                &data
            );
        }
        else {
            throw vk::PushNotUniform();
        }
    }
} // namespace kzn

#endif // KZN_MODEL_RENDERER_HPP