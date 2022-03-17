# Todo

## NOTES

USE noexcept (maybe?)
USE const 
kazan.hpp include with all public API includes

## In Progress
## Todo
- Loading models
- Depth buffering
- Uniform Buffers
    - Descriptor Pools
    - Descriptor Sets 
- Texture mapping
- Generating Mipmaps
- ImGUI Docking test (with example)
- Multisampling
- Allow QueueFamilyIndices to create queues selectively
<!-- Future -->
- Push constants
- Instanced rendering
- Dynamic uniforms
- Separate images and sampler descriptors
- Pipeline cache
- Multi-threaded command buffer generation
- Multiple subpasses
- Compute shaders

## Done
- Remove `VkExtent2D viewport_extent` arg from pipeline.cpp
- Fix resize width cap 
- Work on FIXME's and TODO's:
- Move `include/lib/:dep:` and `src/lib/:dep:` to `lib/:dep:/include` and `lib/:dep:/src`
- Rename to QueueFamilies
- Enable device features

<!--
## In Progress

- Textures
- Refactor code
    - Low level and High level building blocks (Renderer, ...)
    - Distinguish every API bounds (group files by: core, core::platform, utils, math)
    - Error handling
    - Consistent general API with configurability

## Todo 

- ImGUI setup for Kazui
    - Make UI
    - Make Renderer render to a render target (ImGUI Viewport)
- Entt setup

- Lightning
    - PBR (no material hardcoded values)
- Generate Mipmaps
- Multisampling (MXAA)

- Make a check if Swapchain extension(s) are loaded in Swapchain 

- Setup reference docs generation
- Add VK_EXT_extended_dynamic_state and use it

- Enable multiple pipelines at the same time (Entity Component System)

- Refactor code
    - enhance GameObject creation
    - change compiler options to provide better warnings


- Primitive Generator
    - [x] Sphere
    - [x] Box
    - [x] Cylinder
    - [x] Cone
    - [x] Plane
    - [x] Torus
    - [ ] Patches

## See Later
- GPU Driven Rendring (https://vkguide.dev/docs/gpudriven)
- Quaternions rotation
- C++ Auto indent
- PhysicalDeviceProperties is being fetched too many times (fetch once, move ownership, or see which cost os more effective)

## Done
- Refactor code
    - template functionality in headers
- Kazan shader collection
    - Organize shaders folder with dirs with relevant
    names for each shader pairs
    - Make a manual to know the effects of each shader
    pairs and the inputs it requires (maybe provide a
    screenshot too)
-->

## Arquived

- Lightning
    - Array Uniform Buffer (test and finish)
- Fix
    - ! Fix transformations (push constants for now)
        Uniform buffers are bound when the command buffer
        is recording, the update() call changes the buffer
        and every object will use same transformation matrix
    - ! Possible solution, update uniform buffers 

# Example

## Project structure

- include/
    - core/
        - input/
    - vk/
    - math/
    - gui/
    ...
    - kazan.hpp
    - config.hpp
- src/
    - core/
        - input/
    - vk/
    - math/
    - gui/
    ...

## Engine singleton

Single instance class, with core data needed for usage like device.

Problem: Engine needs to be singleton but device may have a configuration stage, where the actual physical device is chosen with properties

Engine::create();

Engine: 
    - Device

Renderer: 
    - time_delta

## Engine usage
```c++
// Implicit Context class as a singleton is initialized by the Renderer
auto window = Window("Window Name", 1700, 800)
// This will initialize Instance, Surface, Device, Swapchain, and everything else
auto renderer = Renderer(&window);

// Load Assets and scene
auto model_1 = Model::load("model.obj");
model_1.set_transform(
    Transform::position(1.0, 0.0, 0.0)
);
auto camera = Camera();
auto scene = Scene { model_1 };

// Initialize default Render pass and pipeline
// If Context is singleton then theres no need
// to have &renderer as argument
auto model_renderer = ModelRenderer(&renderer);

while(!window.should_close())
{
    // Poll events
    window.poll_events();
    // Register 
    auto begin = Time::now();
    // Begin and End frame
    renderer.render_frame([&](){
        model_renderer.bind();
        model_renderer.draw();
        model_renderer.unbind();
    });


    auto end = Time::now();
    // Put FPS in window title
    window.set_title(fmt::format("FPS: {:.0f}", 1 / seconds));
}
```



Alternative window render target API
```c++
auto window = Window(...);
WindowTarget win_target = window.render_target();
renderer.render_to(win_target);
```

## Scene Hierarchy

Node<T> {
    Array<T>
}

Scene {
    Ref<MainCamera>
    Node<Camera>
    Node<Model>
}

## Builder patterns
kzn::Ref<T> reference counted object
```c++
Ref::from()
```



For each complex instantiation
```c++
auto device = DeviceSelector()
    .require_ext(a)
    .select();

auto device = Device()
```

## Device Dependent class instanciation

```c++
// With device as argument
auto device = renderer.device();
Model::load(device, "file.obj")

// With global device
Model::load("file.obj")


auto model = Model::load("file.obj")
renderer.create(model);
```



<!-- Do exceptions in the beginning -->
## Error handling
```c++
Result<T,Error> foo()
{
    return 
}
```

## Pipeline creation
NEEDS REWORK
```c++
DescriptorSetBuilder ds_builder = _pool.descriptor_set_builder();
auto cam_buffer = ds_builder.create_uniform_buffer<CameraData>();
auto lights_buffer = ds_builder.create_uniform_buffer<LightsData>(VK_SHADER_STAGE_FRAGMENT_BIT);
auto set = ds_builder.build();

auto pvm_push = PushConstant<PVMData>(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

auto pipeline_layout = PipelineLayoutBuilder(device)
    .add_descriptor_set(set)
    .add_push_constant(pvm_push)
    .build();

auto pipeline_config =
    PipelineConfig(pipeline_layout, render_pass)
    .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    .set_polygon_mode(VK_POLYGON_MODE_FILL);

auto pipeline = PipelineBuilder(device,"shader.vert","shader.frag")
    .geometry_shader("shader.geom")
    .build();
```

```c++
auto pipeline_layout = PipelineLayoutBuilder(device)
    .build();

auto pipeline_config =
    PipelineConfig(pipeline_layout, render_pass);

auto pipeline = PipelineBuilder(pipeline_config, "shader.vert","shader.frag")
    .build();
```

## Texture
```c++
auto texture = Texture(device, "file.txt")
```

## Material
```c++
// In game loop
auto m = Material(flat_color_shader);
auto mi = MaterialInstance(m);
cube.set_material(mi);

renderer.submit(mi, cube, transform);
```

## Rendering scenes (proposals)

```c++

```

## Swapchain recreation

Window resize must be handled outsize the vk:: handlers

There are two scenarios where the swapchain might need to be recreated,
1. At the beginning of `draw`, when `vkAcquireNextImageKHR` returns `VK_ERROR_OUT_OF_DATE_KHR`
2. At the end of `draw`, when `vkQueuePresentKHR` returns `VK_ERROR_OUT_OF_DATE_KHR` or `VK_SUBOPTIMAL_KHR`
3. When the glfw window Framebuffer resized callback signals that a resize occurred

```c++
// Create RenderPass
auto render_pass = vk::RenderPassBuilder(&device)
    .set_format(swapchain.get_surface_format().format)
    .build();
```
```c++
// Create SwapChain
auto swapchain = vk::SwapchainBuilder(&device, surface, window.extent())
                     .set_present_mode(VK_PRESENT_MODE_FIFO_KHR) // Optional (Default: IMMEDIATE)
                     .build();
```
```c++
// Create Pipeline
auto pipeline_layout = vk::PipelineLayoutBuilder(&device)
                           .build();
auto pipeline_config = vk::PipelineConfigBuilder(pipeline_layout, render_pass, swapchain.get_extent())
                           .build();
auto pipeline = vk::Pipeline(
    &device,
    "assets/shaders/triangle/triangle.vert.spv",
    "assets/shaders/triangle/triangle.frag.spv",
    pipeline_config);
```
Resizing ...
```c++

// 1. 
try {
    swapchain.acquire_next(img_available);
}
catch(const SwapchainResized&) {
    window.set_resized(true);
}

// 3.
if(window.was_resized())
{
    auto win_extent = window.extent();
    swapchain.recreate(win_extent);
    // MUST BE INSIDE PIPELINE BIND
    // pipeline.set_viewport(cmd_buffer, create_viewport(win_extent));
    render_pass.recreate_framebuffers(swapchain);
}

// 2.
try {
    device.present_queue_present(swapchain, finished_render);
}
catch(const SwapchainResized&) {
    swapchain.recreate(window.extent());
    // MUST BE INSIDE PIPELINE BIND
    pipeline.set_viewport(cmd_buffer, create_viewport(win_extent));
    render_pass.recreate_framebuffers(swapchain);
}
```

## Vertex Buffer

```c++
auto vbo = vk::VertexBuffer(&device, buffer_size);
vbo.upload(float*);
vbo.bind(cmd_buffer);

```