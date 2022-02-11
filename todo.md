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

<!-- Today -->
- Lightning
    - Array Uniform Buffer (test and finish) 
    - PBR (no material hardcoded values)

<!----------->

- Logger singleton
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

- Fix
    - ! Fix transformations (push constants for now)
        Uniform buffers are bound when the command buffer
        is recording, the update() call changes the buffer
        and every object will use same transformation matrix
    - ! Possible solution, update uniform buffers 

# Example

## Engine usage
```c++
auto window = Window(1700, 800, "Name")
auto engine = Renderer()
renderer.render_to(window);

// Load Assets and scene
Model::load("models.obj")
```



## Builder patterns
For each complex instantiation
```c++
auto device = DeviceSelector()
    .require_ext(a)
    .select();

auto device = Device()
```