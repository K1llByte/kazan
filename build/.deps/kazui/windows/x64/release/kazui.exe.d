{
    files = {
        [[build\.objs\kazui\windows\x64\release\src\gui\camera_controller.cpp.obj]],
        [[build\.objs\kazui\windows\x64\release\src\gui\main.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_demo.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_draw.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_impl_glfw.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_impl_vulkan.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_tables.cpp.obj]],
        [[build\.objs\imgui\windows\x64\release\lib\imgui\src\imgui_widgets.cpp.obj]],
        [[bin\kazan.lib]]
    },
    values = {
        [[C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.31.31103\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            [[-libpath:C:\VulkanSDK\1.3.216.0\Lib]],
            "-libpath:bin",
            [[-libpath:C:\Users\KillByte\AppData\Local\.xmake\packages\g\glfw\3.3.6\0f1d7b80c6f84759bc07798549faa20b\lib]],
            [[-libpath:C:\Users\KillByte\AppData\Local\.xmake\packages\f\fmt\8.1.1\3c4518782925487388553bdb3459c51d\lib]],
            "vulkan-1.lib",
            "kazan.lib",
            "glfw3.lib",
            "opengl32.lib",
            "fmt.lib",
            "user32.lib",
            "shell32.lib",
            "gdi32.lib"
        }
    }
}