# Kazan

This project is an experimental computer graphics engine for the author to
learn about other graphics APIs and Game Engine architectures.

<!-- An experimental computer graphics engine for learning purposes. -->

## Requirements

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

Library for window creation and managment

```sh
sudo pacman -S glfw
```

<!-- ```sh
# for x11
sudo pacman -S glfw-x11

# or for wayland
sudo pacman -S glfw-wayland
```  -->

### GLM

Header only linear algebra library for computer graphics
```sh
sudo pacman -S glm
```

### Shader Compiler

A compiler and validator from GLSL to SPIR-V (Khronos Group)
```sh
sudo pacman -S glslang
```

___
## References

https://vulkan-tutorial.com/Development_environment#page_Linux