#include "kazan.hpp"

#include "gui/default_pipelines.hpp"

struct Tmp {
    kzn::glsl::vec3 bias1 = glm::vec3{0.0, 1.0, 0.0};
    kzn::glsl::vec3 bias2 = glm::vec3{1.0, 1.0, 1.0};
};

int main(void)
{
    using namespace kzn;
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);

    auto desc_set_layout_cache = vk::DescriptorSetLayoutCache(&Context::device());
    auto desc_set_allocator = vk::DescriptorSetAllocator(&Context::device());

    auto triangle_pass = TrianglePass(&renderer);
    auto triangle_pipeline = create_triangle_pipeline(
        triangle_pass.get_render_pass(),
        desc_set_layout_cache
    );

    auto tmp = Tmp{};
    auto tmp_ubo = vk::UniformBuffer(&Context::device(), sizeof(Tmp));
    tmp_ubo.upload(&tmp);

    auto desc_set0 = desc_set_allocator.allocate(
        triangle_pipeline.descriptor_set_layout(0).vk_layout());
    // desc_set0.update({
    //     tbp_ubo.info()
    // });


    // Models
    // auto monki = Model::load("assets/models/monkey.obj");
    // auto model_pipeline = ModelPipeline();

    // pipeline_manager["phong"]
    // pipeline_manager["phong_wireframe"]



    while(!window.is_closed())
    {
        window.poll_events();

        renderer.render_frame([&](auto& cmd_buffer) {
            // Begin render pass
            triangle_pass.render(cmd_buffer, [&] {
                triangle_pipeline.bind(cmd_buffer);
                vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                vk::cmd_draw(cmd_buffer, 3, 1, 0, 0);

                // triangle_pipeline.bind(cmd_buffer);
                // desc_set.bind(cmd_buffer, triangle_pipeline.layout());
                // vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                // vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                // triangle_pipeline.bind(cmd_buffer);
            });
        });
    }
}