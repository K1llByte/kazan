 # Todo

## In Progress

- [Plan simple editor architecture](api_design.md)

## Todo

- Improve EventManager for better event type deduction (EventManager::listen(on_foo)) and make EventListener class that registers event handlers at runtime 

- [Create Texture and vk::Image abstractions](api_design.md)

- CMake setup shader compiler
- CMake setup vulkan fetch and install SDK
- Github Actions setup CI/CD
- Documentation generation with hdoc/Doxygen+MkDocs/DoxygenAwesomeCss
- Disable some clang-tidy warnings
    - Magic numbers
    - Virtual dtors
    - backslash-newline

- Create Entities panel (list of selectable entities)
- Move vk folder to graphics folder
- Test uniforms without struct
- Move create_sampler/create_image_view utils from render_image.cpp to the vk abstraction layer
- Crude opeartions for SystemManager
- Crude opeartions for RenderGraph
- StringHash constexpr type for instant string comparison and string matching with switch case
- Pseudo RenderGraph
- Improve pipeline layout

## Planned

- Vulkan shader reflection to generate pipeline layout
- Loader classes for loading data from files (PngTextureLoader, ObjMeshLoader)
- Make staged submition for Vertex Buffers
    - For now vertex buffers are in a host visible region on the GPU
    I need to refactor the code to store a staging buffer and the real buffer
    followed by a scheduled copy of the Host visible buffer to the real VertexBuffer
- RenderImage abstraction, probably with a RenderTarget interface
- vk::Queue wrapper (Moveable but not Copyable) with submit method which receives vk::CommandBuffer as argument
- Primitive Generator
- Generate Mipmaps
- Multisampling (MXAA)
- Resource Management. Distinguish engine assets and project assets.
- Uniform structured alignment (have special glsl types)
- Mesh classes with layout description (use glft2 as inspiration)
- Plan 3D and 2D rendering

## Done

## Roadmap #1

- Editor architecture
    - Editor
        - Responsible for intilializing imgui context
        - Owns panels

- ECS Framework using entt
    - Entity/Component/System classes
    - Transform2DComponent
        - Vec2 position
        - float rotation
        - Vec2 scale
    - SpriteComponent
    - Singleton Registry class with an entt registry

- Event System
    - First use case is vulkan events (SwapchainResize)

- RenderSystem
    - PipelineManager, RenderPassManager
    - With a SpriteSubSystem that just renders 2d textures
    - Render images for offscreen rendering (editor viewport )

- Engine Modules
    - Attached and initialized to an application
    - Live throughtout the lifetime of the application
    - Can specify hard dependencies on other modules
    - This will provide an internal reference to those hard dependencies

<!-- 
References:
- GPU Driven Rendering (https://vkguide.dev/docs/gpudriven)
-->