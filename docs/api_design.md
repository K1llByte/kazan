# API Desgin

This document is used as a auxiliary file for todo.md where the API design choices are made.

## Modules example

- Required default ctor
    - After ctor, module state is considered fully initialized


```c++
class GraphicsModule: Module<WindowsModule> {
    GraphicsModule(Window& window);
    ~GraphicsModule() = default;

    vk::Instance&  instance()
    vk::Device&    device()
    vk::Swapchain& swapchain()
};
```

```c++
int main() {
    Module<GraphicsModule>()
}
m_modules.emplace<GraphicsModule>();
```


## Entity usage
```c++
Entity player = Registry::create();
auto& transform2d_comp = player.emplace<Transform2DComponent>(Vec2(1,2));
auto& transform2d_comp = player.get<Transform2DComponent>();
player.remove_component()
Registry::destroy(player)
```

<!--
vk::Queue gfx_queue = graphics_queue()
vk::submit(gfx_queue, wait_semaphore)

vk::immediate_submit(gfx_queue, [](){

})
-->



## Event System
```c++
struct WindowResized: Event {};
struct SwapchainResize: Event {};

using ResizeEvents = MultipleEvents<
    WindowResize,
    SwapchainResize
>;

void on_swapchain_resize(const ComponentAdded& event);
void on_resize_events(const ResizeEvents& event) {
    event.visit(
        [](const SwapchainResized&) {},
        [](const WindowResized&) {},
    );
}

// NOTE: This might need forward declaration
class Foo;
using FooListener = EventListener<
    Foo::on_swapchain_resize,
    Foo::on_resize_events,
>;

// Will automatically register/unregister event handlers
struct Foo: FooListener {
};

// Will automatically register/unregister event handlers
struct Foo: FooListener {
    Foo() {
        // Adds listeners dynamically,
        // removes them once dtor is called.
        listen(&Foo::on_component_added);
        listen(&Foo::on_resize_events);
        unlisten(&Foo::on_resize_events);
    }
};

int main() {
    EventManager::listen(on_component_added);
    EventManager::listen(on_swapchain_resize);
}
```


## Image
```c++
auto img = vk::Image(&device, {width,height}, vk::ImageFormat::RGBA8);
```


----------------------------------------------
<!----------------- OLD --------------------->
______________________________________________

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

## Compute indices of vertices
```c++
auto vbo = vk::VertexBuffer({ /*...*/ });
auto model = Model::with_computed_indices(vbo);

```

## UniformBuffer
```c++
auto ubo = vk::UniformBuffer(&device, buffer_size);
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


<!-- Do exceptions in the beginning -->
## Error handling
```c++
Result<T,Error> foo()
{
    return 
}
```

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

while(!window.is_closed())
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
