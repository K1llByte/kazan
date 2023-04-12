#pragma once

#include "core/events/event_handlers.hpp"
#include "vk/pipeline.hpp"
#include "vk/framebuffer.hpp"
#include "vk/commands.hpp"
#include "core/context.hpp"

namespace kzn
{
    class ScreenDepthPass: public EventHandlers
    {
    public:
        ScreenDepthPass();
        ~ScreenDepthPass() = default;

        template<typename F>
            requires std::is_invocable_r_v<void, F>
        void render(vk::CommandBuffer& cmd_buffer, F&& func);
        vk::RenderPass& get_render_pass() { return render_pass; }

        void on_resize(const ResizeEvent&);

    private:
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
    void ScreenDepthPass::render(vk::CommandBuffer& cmd_buffer, F&& func)
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