#define KZN_LOG_LEVEL 5
#include "kazan.hpp"

#include "gui/default_pipelines.hpp"
#include "gui/camera_controller.hpp"

int main(void)
{
    using namespace kzn;
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);
    auto desc_set_layout_cache = vk::DescriptorSetLayoutCache(&Context::device());
    auto desc_set_allocator = vk::DescriptorSetAllocator(&Context::device());
 
    auto depth_pass = SimpleDepthPass(&renderer);

    auto mesh_pipeline = create_mesh_pipeline(
        depth_pass.get_render_pass(),
        desc_set_layout_cache
    );

    // Models
    auto monkey = Model::load("assets/models/monkey.obj");
    auto viking_room = Model::load("assets/models/viking_room.obj");
    viking_room.transform.rotation.z += glm::radians(90.f);
    viking_room.transform.rotation.y += glm::radians(90.f);
    viking_room.transform.position.z += 4.f;

    auto camera = Camera::perspective(50.f, window.aspect_ratio(), 0.1f, 100.f);
    camera.lookat_target({10.f, 2.f, 0.f}, {0.f, 0.f, 0.f});

    auto pvm_monkey = PVM{
        camera.projection() * camera.view(),
        monkey.transform.mat4()
    };
    auto pvm_viking_room = PVM{
        camera.projection() * camera.view(),
        viking_room.transform.mat4()
    };

    auto controller = CameraController(&window, &camera);

    // auto pvm_ubo = vk::UniformBuffer(&Context::device(), sizeof(PVM));
    // pvm_ubo.upload(&pvm);

    // auto desc_set0 = desc_set_allocator.allocate(
    //     triangle_pipeline.descriptor_set_layout(0));
    // desc_set0.update({
    //     vk::DescriptorInfo{tmp_ubo.info()}
    // });

    while(!window.is_closed())
    {
        window.poll_events();

        // Update
        controller.update(0.016);
        pvm_monkey.proj_view = camera.projection() * camera.view();
        pvm_viking_room.proj_view = camera.projection() * camera.view();

        // if(window.was_resized()) {
        //     camera.set_perspective(50.f, window.aspect_ratio(), 0.1f, 100.f);
        // }

        // Render
        renderer.render_frame([&](auto& cmd_buffer) {
            // Begin render pass
            depth_pass.render(cmd_buffer, [&] {
                vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                mesh_pipeline.bind(cmd_buffer);
                // TODO: add constraints to this cmd
                // Model 1
                vk::cmd_push_constants(cmd_buffer, mesh_pipeline.layout(), pvm_monkey);
                monkey.draw(cmd_buffer);
                // Model 2
                vk::cmd_push_constants(cmd_buffer, mesh_pipeline.layout(), pvm_viking_room);
                viking_room.draw(cmd_buffer);

                // triangle_pipeline.bind(cmd_buffer);
                // desc_set.bind(cmd_buffer, triangle_pipeline.layout());
                // vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                // vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                // triangle_pipeline.bind(cmd_buffer);
            });
        });
    }
}