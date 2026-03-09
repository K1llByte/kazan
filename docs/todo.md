
# Todo

- [ ] Create vk::CubeImage
- [ ] Skybox shader
- [ ] Runtime shader compilation (with glslc's libshaderc)
- [ ] FIXME: default material being always allocated
- [ ] Registry `find_with<C1, C2, ...>()` method
- [ ] Fix problem with glsl data alignment in Camera3DUniformData, cant have a Vec3 before a Float
- [ ] Mesh loader
- [ ] 3D Graphics rendering pipeline
- [ ] Add volk meta loader as dependency
- [ ] Remove Singleton base class
- [ ] Reuse simple imgui overlay to display text information such as rendering info
- [ ] Make vulkan features configurable for any vulkan version
- [ ] Add https://github.com/martinus/unordered_dense as dependency and benchmark performance differences

_______________________________________________________________________________
# Planned

- Consider using https://github.com/abseil/abseil-cpp
- Separate git LFS repo with assets
- Modern cmake presets
- Profile rendering with amd/nvidia profiler
- RenderImage abstraction, probably with a RenderTarget interface
- vk::Queue wrapper (Moveable but not Copyable) with submit method which receives vk::CommandBuffer as argument
- Primitive Generator
- Mipmaps
- Anisotropic filtering
- Multisampling (MXAA)
- Hierarchy component
- Make SpriteGeometryCache work like a ref counted allocator, when theres no more references left, destroy the geometry
- Figure out how to submit mat3 uniform data (alignment not correct)
- Fix alpha test not working for alpha textures between players texture (if players are in same depth)
- Bug: Physics segfaults if theres a system that runs before it, that initializes a physics component
    - This is a current design flaw of components
- Make EventHandlerId a mix of the event type id + counter
- Write kzn cli to create applications, compile shaders and compile project

_______________________________________________________________________________
# Done

- [x] Basic CameraSystem controller 
- [x] Camera3D component
- [x] Move DsetLayoutCache and Allocator to Device instead of Renderer
    - Allows PipelineBuilder to reuse these constructs
- [x] Make pipeline allow query its descriptor set layouts by index
    - pipeline.dset_layout(0)
- [x] PipelineBuilder that uses spirv reflection data to deduce pipeline layout
- [x] Fix vkQueueSubmit warning
- [x] Slang shader support
- [x] Add versioned cmake dependencies
- [x] Refactor CMakeLists.txt to allow adding kazan/paperlib as a submodule and write a simple application using engine
- [x] Make BitMayhem compile again
- [-] Disable warnings for external libs
- [x] Unify paper and kazan into same lib
- [x] BasicApp base class
- [x] Test example that just displays a texture
- [x] Change console colors
- [x] Simple system scheduler (DAG + topologycal sort)
    - [x] Express dependencies using associated type `using Before = TypeList<RenderSystem>`
- [x] Refactor Renderer into a non singleton
- [x] Refactor Registry into a non singleton
- [x] Make EditorSystem only initialize after RenderSystem, since it depends on it and the renderer
- [x] Brainstorm how to deal with relative engine/app path
- [x] Brainstorm a way to costumize render stages in the RenderSystem