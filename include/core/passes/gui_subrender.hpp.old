#ifndef KZN_GUI_SUBRENDER_HPP
#define KZN_GUI_SUBRENDER_HPP

#include "core/renderer.hpp"
#include "vk/render_pass.hpp"
#include "vk/framebuffer.hpp"
// #include "core/texture.hpp"
// #include "vk/pipeline.hpp"
// #include "vk/descriptor_set.hpp"
// #include "vk/buffers.hpp"

namespace kzn {
    class GuiSubrender {
        public:
        GuiSubrender(Renderer* _renderer);
        ~GuiSubrender() = default;

        void begin(vk::CommandBuffer& cmd_buffer);
        void end(vk::CommandBuffer& cmd_buffer);

    private:
        Renderer*         renderer;
        vk::RenderPass    render_pass;
        vk::Framebuffers  framebuffers;        
    };
} // namespace kzn

#endif // KZN_MODEL_RENDERER_HPP