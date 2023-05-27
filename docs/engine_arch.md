# Engine architecture

## Project Structure

- core/
    - logger
    - asserts
    - memory
- math/
- events/
- entity/
- physics/
- ai/
- graphics/
    - vk/
- editor/
    - main.cpp

// systems/
// graphics/
// - vk/
// - passes/
// - renderer.hpp
// - graphics_context.hpp
// math/
// - coordinates.hpp

## Exposed dependencies

- glm
- stl

## Renderer design

```cpp
auto renderer = Renderer();
// Renderer() // initializes
// ~Renderer() 
renderer.begin_frame()
renderer.end_frame()
```

## 