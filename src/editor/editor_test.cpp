#include "kazan.hpp"

#include <cmath>

// TODO: Create alias type in core/ Ref = std::reference_wrapper
// and change every non_owning ptr to Ref 

using namespace kzn;


vk::Pipeline triangle_pipeline(vk::RenderPass& render_pass) {
    return vk::Pipeline(
        render_pass.device(),
        vk::PipelineStages{
            .vertex = "assets/shaders/triangle/triangle.vert.spv",
            .fragment = "assets/shaders/triangle/triangle.frag.spv",
        },
        vk::PipelineConfig(render_pass)
    );
}

int main() try {
    auto window = Window("Kazan Engine", 1000, 800);

    auto instance = vk::Instance({
        .extensions = window.required_extensions(),
        .with_validation = true,
    });
    auto surface = window.create_surface(instance);
    auto device = vk::Device(instance, {
        .extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        .surface = surface.vk_surface()
    });
    auto swapchain = vk::Swapchain(device, surface, window.extent());

    auto render_pass = simple_pass(device, swapchain.image_format());
    auto framebuffers = create_swapchain_framebuffers(render_pass, swapchain);

    auto pipeline = triangle_pipeline(render_pass);

    auto renderer = Renderer(device, swapchain, window);

    renderer.on_swapchain_resize([&] {
        framebuffers = create_swapchain_framebuffers(render_pass, swapchain);
    });

    size_t frame_counter = 0;
    while(!window.is_closed()) {
        window.poll_events();

        // Render
        renderer.render_frame([&](auto& cmd_buffer) {
            // Render pass
            render_pass.begin(
                cmd_buffer,
                framebuffers[swapchain.current_index()],
                { VkClearValue{{{0.01f, 0.01f, 0.01f, 1.0f}}} });

            vk::cmd_set_viewport(cmd_buffer, vk::create_viewport(swapchain.extent()));
            vk::cmd_set_scissor(cmd_buffer, vk::create_scissor(swapchain.extent()));
            
            pipeline.bind(cmd_buffer);

            vk::cmd_draw(cmd_buffer, 3);
            render_pass.end(cmd_buffer);
        });

        // Increment frame counter
        frame_counter += 1;
    }

    device.wait_idle();
}
catch(vk::ResultError re) {
    Log::error("{}", re.message());
}