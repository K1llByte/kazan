// #include "kazan.hpp"

// #include <cmath>

// // TODO: Create alias type in core/ Ref = std::reference_wrapper
// // and change every non_owning ptr to Ref 

// using namespace kzn;

// vk::Pipeline triangle_pipeline(vk::RenderPass& render_pass, const vk::DescriptorSetLayout& dset_layout) {
//     return vk::Pipeline(
//         render_pass.device(),
//         vk::PipelineStages{
//             .vertex = "assets/shaders/triangle_ubo/triangle.vert.spv",
//             .fragment = "assets/shaders/triangle_ubo/triangle.frag.spv",
//         },
//         vk::PipelineConfig(render_pass)
//             .set_vertex_input<glm::vec2, glm::vec3>()
//             .set_layout({
//                 .descriptor_sets = { dset_layout.vk_layout() }
//             })
//     );
// }

// struct Vertex {
//     glm::vec2 position;
//     glm::vec3 color;
// };

// struct Pvm {
//     glm::vec2 shift;
// };

// int main() try {
//     auto window = Window("Kazan Engine", 1000, 800);

//     auto instance = vk::Instance({
//         .extensions = window.required_extensions(),
//         .with_validation = true,
//     });
//     auto surface = window.create_surface(instance);
//     auto device = vk::Device(instance, {
//         .extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
//         .surface = surface.vk_surface()
//     });
//     auto swapchain = vk::Swapchain(device, surface, window.extent());

//     auto render_pass = simple_pass(device, swapchain.image_format());
//     auto framebuffers = create_swapchain_framebuffers(render_pass, swapchain);

//     auto dset_allocator = vk::DescriptorSetAllocator(device);
//     auto dset_cache = vk::DescriptorSetLayoutCache(device);
//     auto dset_layout_0 = dset_cache.create_layout({
//         vk::uniform_binding(0),
//     });
//     auto pipeline = triangle_pipeline(render_pass, dset_layout_0);

//     auto dset_0 = dset_allocator.allocate(std::move(dset_layout_0));
//     auto pvm_ubo = vk::UniformBuffer(device, sizeof(Pvm));
//     auto pvm = Pvm{ glm::vec2{1.f} };
//     pvm_ubo.upload(&pvm);
//     dset_0.update({ pvm_ubo.info() });

//     auto renderer = Renderer(device, swapchain, window);

//     renderer.on_swapchain_resize([&] {
//         framebuffers = create_swapchain_framebuffers(render_pass, swapchain);
//     });

//     const std::vector<Vertex> vertices = {
//         {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//         {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//         {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//         {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//     };
//     const std::vector<uint32_t> indices = {
//         0, 1, 2, 2, 3, 0
//     };

//     auto vertex_buffer = vk::VertexBuffer(device, sizeof(Vertex) * vertices.size());
//     vertex_buffer.upload(reinterpret_cast<const float*>(vertices.data()));
//     auto index_buffer = vk::IndexBuffer(device, sizeof(uint32_t) * indices.size());
//     index_buffer.upload(indices.data());

//     size_t frame_counter = 0;
//     while(!window.is_closed()) {
//         window.poll_events();

//         // Render
//         renderer.render_frame([&](auto& cmd_buffer) {
//             // Render pass
//             float brightness = 0.5f + 0.5f * std::sin(0.001f * float(frame_counter));
//             render_pass.begin(
//                 cmd_buffer,
//                 framebuffers[swapchain.current_index()],
//                 { VkClearValue{{{brightness, brightness, brightness, 1.0f}}} });

//             //vk::cmd_set_viewport(cmd_buffer, renderer.current_viewport());
//             //vk::cmd_set_scissor(cmd_buffer, renderer.current_scissor());
//             // // TODO: Utilities for scissor and viewport
//             VkViewport viewport{};
//             viewport.x = 0.0f;
//             viewport.y = 0.0f;
//             viewport.width = static_cast<float>(swapchain.extent().width);
//             viewport.height = static_cast<float>(swapchain.extent().height);
//             viewport.minDepth = 0.0f;
//             viewport.maxDepth = 1.0f;
//             vkCmdSetViewport(cmd_buffer.vk_cmd_buffer(), 0, 1, &viewport);

//             VkRect2D scissor{};
//             scissor.offset = {0, 0};
//             scissor.extent = swapchain.extent();
//             vkCmdSetScissor(cmd_buffer.vk_cmd_buffer(), 0, 1, &scissor);
            
//             pipeline.bind(cmd_buffer);
//             vertex_buffer.bind(cmd_buffer);
//             index_buffer.bind(cmd_buffer);
//             dset_0.bind(cmd_buffer, pipeline.layout());
//             vkCmdDrawIndexed(cmd_buffer.vk_cmd_buffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
//             // vkCmdDraw(cmd_buffer.vk_cmd_buffer(), 3, 1, 0, 0);
//             render_pass.end(cmd_buffer);
//         });

//         // Increment frame counter
//         frame_counter += 1;
//     }

//     device.wait_idle();
// }
// catch(vk::ResultError re) {
//     Log::error("{}", re.message());
// }