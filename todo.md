# Todo

## In Progress

- Refactor code
    - change implicit conversions

## Todo 


<!-- Today -->
- Lightning
    - Array Uniform Buffer 
    - PBR (no material hardcoded values)

<!----------->

- Add VK_EXT_extended_dynamic_state and use it

- Enable multiple pipelines at the same time (MultipleRenderSystem)

- Refactor code
    - enhance GameObject creation
    - change compiler options to provide better warnings
    - make Kazan prepared for basic engine usage

- ImGui Commands GUI
- ImGui Logger GUI

- Textures

- GPU Driven Rendring (https://vkguide.dev/docs/gpudriven)
    - Use transformation matrices from uniform buffers

- Primitive Generator
    - [x] Sphere
    - [x] Box
    - [x] Cylinder
    - [x] Cone
    - [x] Plane
    - [x] Torus
    - [ ] Patches

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