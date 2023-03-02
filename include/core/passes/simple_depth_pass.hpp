#pragma once

#include "core/renderer.hpp"
#include "core/events/event_handlers.hpp"
#include "vk/pipeline.hpp"
#include "vk/framebuffer.hpp"
#include "vk/commands.hpp"

namespace kzn
{
    class SimpleDepthPass: public EventHandlers
    {
    public:
        SimpleDepthPass(Renderer* _renderer);
        ~SimpleDepthPass() = default;

        template<typename F>
            requires std::is_invocable_r_v<void, F>
        void render(vk::CommandBuffer& cmd_buffer, F&& func);
        vk::RenderPass& get_render_pass() { return render_pass; }

        void on_resize(const ResizeEvent&);

    private:
        Renderer*        renderer;
        vk::RenderPass   render_pass;
        vk::Framebuffers framebuffers;
    };
} // namespace kzn

////////////////////////////////////////////////
//             Implementation                 //
////////////////////////////////////////////////

namespace kzn
{
    template<typename F>
        requires std::is_invocable_r_v<void, F>
    void SimpleDepthPass::render(vk::CommandBuffer& cmd_buffer, F&& func)
    {
        //////////////////////
        // Begin RenderPass //
        //////////////////////

        render_pass.begin(
            cmd_buffer,
            framebuffers.get(Context::swapchain().current_index())
        );

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