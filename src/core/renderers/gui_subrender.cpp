#include "core/renderers/gui_subrender.hpp"

namespace kzn {
    GuiSubrender::GuiSubrender(Renderer* _renderer)
        : renderer(_renderer)
        // Init render pass 
        , render_pass(vk::simple_depth_render_pass(
            Context::device(),
            Context::swapchain().get_surface_format().format))
        // Init framebuffers
        , framebuffers(
            &Context::device(),
            &render_pass,
            Context::swapchain().images(),
            Context::swapchain().get_extent())
        {
            // Set callback to recreate framebuffers on swapchain resize
            renderer->on_swapchain_resize([&]() {
                // Recreate framebuffers from swapchain
                framebuffers.recreate(
                    Context::swapchain().images(),
                    Context::swapchain().get_extent()
                );
            });
            // Add render pass to renderer
            renderer->add_render_pass(render_pass);
        }

    void GuiSubrender::begin(vk::CommandBuffer& cmd_buffer) {
        render_pass.begin(
            cmd_buffer,
            framebuffers.get(Context::swapchain().current_index()));
    }


    void GuiSubrender::end(vk::CommandBuffer& cmd_buffer) {
        render_pass.end(cmd_buffer);
    }


} // namespace kzn