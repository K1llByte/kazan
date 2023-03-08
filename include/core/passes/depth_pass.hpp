#pragma once

#include "core/events/event_handlers.hpp"
#include "gui/editor_events.hpp"
#include "vk/framebuffer.hpp"
#include "vk/commands.hpp"
#include "core/context.hpp"

#include "core/render_image.hpp"

namespace kzn
{
    class DepthPass: public EventHandlers
    {
    public:
        DepthPass(RenderImage* render_image);
        ~DepthPass() = default;

        template<typename F>
            requires std::is_invocable_r_v<void, F>
        void render(vk::CommandBuffer& cmd_buffer, F&& func);

        vk::RenderPass& get_render_pass() { return m_render_pass; }

        void on_viewport_resize(const ViewportResizeEvent&);

    private:
        RenderImage*     m_render_image;
        vk::RenderPass   m_render_pass;
        vk::Framebuffers m_framebuffers;
    };
} // namespace kzn

////////////////////////////////////////////////
//             Implementation                 //
////////////////////////////////////////////////

namespace kzn
{
    template<typename F>
        requires std::is_invocable_r_v<void, F>
    void DepthPass::render(vk::CommandBuffer& cmd_buffer, F&& func)
    {
        //////////////////////
        // Begin RenderPass //
        //////////////////////

        m_render_pass.begin(
            cmd_buffer,
            m_framebuffers.get(0)
        );

        ///////////////////
        // Register Cmds //
        ///////////////////

        std::invoke(std::forward<F>(func));

        ////////////////////
        // End RenderPass //
        ////////////////////

        m_render_pass.end(cmd_buffer);
    }
} // namespace kzn