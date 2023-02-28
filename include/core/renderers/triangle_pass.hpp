#pragma once

#include "core/renderer.hpp"
#include "core/events/event_handlers.hpp"
#include "vk/pipeline.hpp"
#include "vk/framebuffer.hpp"
#include "vk/commands.hpp"

namespace kzn
{
    class TrianglePass: public EventHandlers
    {
    public:
        TrianglePass(Renderer* _renderer);
        ~TrianglePass() = default;

        template<typename F>
            requires std::is_invocable_r_v<void, F>
        void render(vk::CommandBuffer& cmd_buffer, F&& func);

        void on_resize(const ResizeEvent&);

    private:
        Renderer*        renderer;
        vk::RenderPass   render_pass;
        vk::Framebuffers framebuffers;
        vk::Pipeline     pipeline;
    };
} // namespace kzn

////////////////////////////////////////////////
//             Implementation                 //
////////////////////////////////////////////////

namespace kzn
{
    template<typename F>
        requires std::is_invocable_r_v<void, F>
    void TrianglePass::render(vk::CommandBuffer& cmd_buffer, F&& func)
    {
        //////////////////////
        // Begin RenderPass //
        //////////////////////

        render_pass.begin(
            cmd_buffer,
            framebuffers.get(Context::swapchain().current_index())
        );

        auto& viewport = renderer->current_viewport();
        auto& scissor = renderer->current_scissor();
        
        pipeline.bind(cmd_buffer);
        vk::cmd_set_viewport(cmd_buffer, viewport);
        vk::cmd_set_scissor(cmd_buffer, scissor);

        ///////////////////
        // Register Cmds //
        ///////////////////

        std::invoke(std::forward<F>(func));

        ////////////////////
        // End RenderPass //
        ////////////////////

        render_pass.end(cmd_buffer);
    }
} // namespace kzn