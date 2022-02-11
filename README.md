# Kazan

This project is an experimental computer graphics engine for experimentation and learning.

<!--
- Computer Graphics
- Vulkan graphics API
- Game Engine Architectures
- Game development
-->

![](https://i.imgur.com/u8C2qSc.png)

<!-- An experimental computer graphics engine for learning purposes. -->

## Requirements

- Vulkan Devel SDK
- GLFW
- GLM
- Shader SPIR-V compiler (Optional)

<!-- To install everything needed:
```sh
sudo pacman -S vulkan-devel glfw glm glslang
```

### Vulkan Packages

All this vulkan packages can be installed with pacman by installing the vulkan
development package.
```sh
sudo pacman -S vulkan-devel
```

- **Command-line utilities** - Test if vulkan is functional.
```sh
sudo pacman -S vulkan-tools
```

- **Vulkan loader** - Looks up the functions in the driver at runtime.
```sh
sudo pacman -S vulkan-icd-loader
```
> Note: `vulkan-tools`  `vulkan-validation-layers` packages already include the
ICD Loader

- **Validation layers & SPIR-V tools** - Standard validation layers and SPIR-V
tools.
```sh
sudo pacman -S vulkan-validation-layers spirv-tools
```

### GLFW

Library for window creation and managment & Input events

```sh
sudo pacman -S glfw
```

### GLM

Header only linear algebra library for computer graphics
```sh
sudo pacman -S glm
``` -->

<!-- ### Shader Compiler

A compiler and validator from GLSL to SPIR-V (Khronos Group)
```sh
sudo pacman -S glslang
```

Or alternatively the glslc compiler with clang like usage (Google)
```sh
sudo pacman -S shaderc
``` -->

## Building

Before trying the engine out it need to be compiled, and for that `xmake`
is required for building this project.

There are some requirements for compilation, but `xmake` will install all missing dependencies,
if something doesn't work or cant be installed automatically by xmake, you can install manually any
required dependency listed [here](#requirements)
```sh
git clone git@github.com:K1llByte/kazan.git
cd kazan
xmake build
```

## Demo

To run a demo of the kazan engine after compilation just run:
```sh
./bin/renderer
```


___
## References

https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/pdf/vkspec.pdf
https://www.khronos.org/files/vulkan11-reference-guide.pdf
https://vulkan-tutorial.com/Development_environment#page_Linux
https://vkguide.dev/
https://learnopengl.com/Lighting/Multiple-lights