# Todo

## In Progress

- Textures
- Refactor code


    - Error handling
    - Init and cleanup consistency
    - Consistent general API with configurability
    - Low level and High level building blocks (Renderer, ...)
    - change implicit conversions

## Todo 

- ImGUI setup for Kazui
    - Make UI
    - Make Renderer render to a render target (ImGUI Viewport)
- Entt setup

<!-- Today -->
- Lightning
    - PBR (no material hardcoded values)
- Generate Mipmaps
- Multisampling (MXAA)
<!----------->

- Quick: Change "Logger" class to "Log"
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

## Engine singleton

Single instance class, with core data needed for usage like device.

Engine: 
    - Device

Renderer: 
    - time_delta


## Engine usage
```c++
auto window = Window(1700, 800, "Name")
auto renderer = Renderer()
auto device = renderer.device();
renderer.render_to(window);

// Load Assets and scene
auto model_1 = Model::load("model.obj")
auto scene = Scene {

};


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