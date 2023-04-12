# Kazan

This project is an experimental computer graphics engine for experimentation and learning.

<!--
- Computer Graphics
- Vulkan graphics API
- Game Engine Architectures
- Game development
-->

![](https://i.imgur.com/miT8j6t.png)
<!-- ![](https://i.imgur.com/u8C2qSc.png) -->

## Requirements

- Vulkan Devel SDK
- GLFW
- GLM
- Shader SPIR-V compiler (Optional)

## Building

There are some requirements for compilation, but `xmake` will install all missing dependencies,
if something doesn't work or cant be installed automatically by xmake, you can install manually any
required dependency listed [here](#requirements)
```sh
git clone git@github.com:K1llByte/kazan.git
cd kazan
xmake build kazui
```

## Demo

To run a demo of the kazan engine after compilation just run:
```sh
./bin/kazui
```


___
## References

https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/pdf/vkspec.pdf  
https://www.khronos.org/files/vulkan11-reference-guide.pdf  
https://vulkan-tutorial.com/Development_environment#page_Linux  
https://vkguide.dev/  
https://learnopengl.com/Lighting/Multiple-lights  