#include "kazan.hpp"

#include "gui/default_pipelines.hpp"
#include "gui/camera_controller.hpp"

using namespace kzn;

// struct Tmp {
//     glsl::vec3 bias1 = glm::vec3{1,0,1};
//     glsl::vec3 bias2 = glm::vec3{0,0,1};
// };


// int main(void)
// {
//     auto window = Window("Kazui", 1000, 800);
//     auto renderer = Renderer(&window);
//     auto desc_set_layout_cache = vk::DescriptorSetLayoutCache(&Context::device());
//     auto desc_set_allocator = vk::DescriptorSetAllocator(&Context::device());

//     auto depth_pass = ScreenDepthPass();

//     auto mesh_pipeline = create_mesh_pipeline(
//         depth_pass.get_render_pass(),
//         desc_set_layout_cache
//     );

//     // Models
//     auto viking_room = Model::load("assets/models/viking_room.obj");
//     viking_room.transform.rotation.z += glm::radians(90.f);
//     viking_room.transform.rotation.y += glm::radians(90.f);

//     auto camera = Camera::perspective(50.f, window.aspect_ratio(), 0.1f, 100.f);
//     camera.lookat_target({10.f, 2.f, 0.f}, {0.f, 0.f, 0.f});
//     // TMP: auto camera_resizer = CameraResizer(&camera, &window);

//     auto pvm_viking_room = PVM{
//         camera.projection() * camera.view(),
//         viking_room.transform.mat4()
//     };

//     auto controller = CameraController(&window, &camera);

//     auto viking_room_tex = Texture::load("assets/textures/viking_room.png");
//     auto viking_room_image = vk::Image(&Context::device(), viking_room_tex.get_extent());
//     viking_room_image.upload(viking_room_tex.get_data());


//     auto desc_set_0 = desc_set_allocator.allocate(
//         mesh_pipeline.descriptor_set_layout(0));
//     desc_set_0.update({
//         viking_room_image.info()
//     });

//     while(!window.is_closed())
//     {
//         window.poll_events();

//         // Update //
//         controller.update(0.016);
//         pvm_viking_room.proj_view = camera.projection() * camera.view();

//         // Render //
//         renderer.render_frame([&](auto& cmd_buffer) {
//             // Begin render pass
//             depth_pass.render(cmd_buffer, [&] {
//                 vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
//                 vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
//                 mesh_pipeline.bind(cmd_buffer);
//                 desc_set_0.bind(cmd_buffer, mesh_pipeline.layout());
//                 // TODO: add type constraints to this cmd
//                 // Model 1
//                 vk::cmd_push_constants(cmd_buffer, mesh_pipeline.layout(), pvm_viking_room);
//                 viking_room.draw(cmd_buffer);
//             });
//         });

//         // editor_pass.render(cmd_buffer, [] {

//         // });
//     }
// }


// Testing ImGui
#include "gui/editor.hpp"

int main(void)
{
    auto window = Window("Kazui", 1000, 800);
    auto renderer = Renderer(&window);
    auto desc_set_layout_cache = vk::DescriptorSetLayoutCache(&Context::device());
    auto desc_set_allocator = vk::DescriptorSetAllocator(&Context::device());

    auto screen_depth_pass = ScreenDepthPass();
    auto editor = Editor(window, screen_depth_pass);
    auto depth_pass = DepthPass(editor.viewport_image());
    
    ///////////////////////////////
    auto mesh_pipeline = create_mesh_pipeline(
        depth_pass.get_render_pass(),
        desc_set_layout_cache
    );

    // Models
    auto viking_room = Model::load("assets/models/viking_room.obj");
    viking_room.transform.rotation.z += glm::radians(90.f);
    viking_room.transform.rotation.y += glm::radians(90.f);

    auto camera = Camera::perspective(50.f, window.aspect_ratio(), 0.1f, 100.f);
    camera.lookat_target({10.f, 2.f, 0.f}, {0.f, 0.f, 0.f});
    // TMP: auto camera_resizer = CameraResizer(&camera, &window);

    auto pvm_viking_room = PVM{
        camera.projection() * camera.view(),
        viking_room.transform.mat4()
    };

    auto viking_room_tex = Texture::load("assets/textures/viking_room.png");
    auto viking_room_image = vk::Image(&Context::device(), viking_room_tex.get_extent());
    viking_room_image.upload(viking_room_tex.get_data());

    auto desc_set_0 = desc_set_allocator.allocate(
        mesh_pipeline.descriptor_set_layout(0));
    desc_set_0.update({
        viking_room_image.info()
    });


    while(!window.is_closed())
    {
        window.poll_events();

        // Render //
        renderer.render_frame([&](auto& cmd_buffer) {
            // Geometry pass
            depth_pass.render(cmd_buffer, [&] {
                vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
                vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
                mesh_pipeline.bind(cmd_buffer);
                desc_set_0.bind(cmd_buffer, mesh_pipeline.layout());
                // TODO: add type constraints to this cmd
                // Model 1
                vk::cmd_push_constants(cmd_buffer, mesh_pipeline.layout(), pvm_viking_room);
                viking_room.draw(cmd_buffer);
            });

            // Editor pass
            screen_depth_pass.render(cmd_buffer, [&] {
                editor.draw(cmd_buffer);
            });
        });
    }
}