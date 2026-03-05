# Kazan

Modern C++ 20 vulkan rendering engine.

![](https://i.imgur.com/miT8j6t.png)

## Building

```sh
# Clone repository
git clone git@github.com:K1llByte/kazan.git
cd kazan

# Generate and compile project
cmake -Bbuild && cmake --build build

# Compile shaders before running executable
./compile_shaders.sh

# Run test executable
./bin/test
```

<!--
_______________________________________________________________________________
## References
- GPU Driven Rendering https://vkguide.dev/docs/gpudriven
- https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/pdf/vkspec.pdf  
- https://www.khronos.org/files/vulkan11-reference-guide.pdf  
- https://vulkan-tutorial.com/Development_environment#page_Linux  
- https://vkguide.dev/  
- https://learnopengl.com/
-->