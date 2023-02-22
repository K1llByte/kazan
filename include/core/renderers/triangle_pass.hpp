#ifndef KZN_TRIANGLE_RENDERER_HPP
#define KZN_TRIANGLE_RENDERER_HPP

#include "core/renderer.hpp"
#include "core/events/event_handlers.hpp"
#include "vk/pipeline.hpp"
#include "vk/framebuffer.hpp"

namespace kzn
{
    class TrianglePass: public EventHandlers
    {
    public:
        TrianglePass(Renderer* _renderer);
        ~TrianglePass() = default;

        void render(vk::CommandBuffer& cmd_buffer, std::function<void(void)> func);
        void draw(vk::CommandBuffer& cmd_buffer);

        void on_resize(const ResizeEvent&);

    private:
        Renderer*        renderer;
        vk::RenderPass   render_pass;
        vk::Framebuffers framebuffers;
        vk::Pipeline     pipeline;
    };
} // namespace kzn

#endif // KZN_TRIANGLE_RENDERER_HPP