{
    depfiles_cl_json = "{\
    \"Version\": \"1.1\",\
    \"Data\": {\
        \"Source\": \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\src\\\\core\\\\input.cpp\",\
        \"ProvidedModule\": \"\",\
        \"Includes\": [\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\core\\\\input.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glfw\\\\3.3.6\\\\0f1d7b80c6f84759bc07798549faa20b\\\\include\\\\glfw\\\\glfw3.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\stddef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\sal.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\concurrencysal.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vadefs.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\stdint.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vulkan.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vk_platform.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vulkan_core.h\"\
        ],\
        \"ImportedModules\": [],\
        \"ImportedHeaderUnits\": []\
    }\
}",
    files = {
        [[src\core\input.cpp]]
    },
    values = {
        [[C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.31.31103\bin\HostX64\x64\cl.exe]],
        {
            "/EHsc",
            "-nologo",
            "-W4",
            "-Ox",
            "-fp:fast",
            "-std:c++20",
            "-Iinclude",
            [[-Ilib\tiny_obj_loader\include]],
            [[-Ilib\vma\include]],
            "-DGLFW_INCLUDE_NONE",
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\v\vulkan-headers\1.2.198+0\06123fbd318a4a08a4b63d4c12e153b5\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\g\glfw\3.3.6\0f1d7b80c6f84759bc07798549faa20b\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\g\glm\0.9.9+8\8335bab7c0314ab9921440de58961aac\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\f\fmt\8.1.1\3c4518782925487388553bdb3459c51d\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\Desktop\Documents\cmake_test\vcpkg\installed\x64-windows-static\include]],
            "/Zc:preprocessor"
        }
    }
}