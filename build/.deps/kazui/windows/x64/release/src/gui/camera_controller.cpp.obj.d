{
    values = {
        [[C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.31.31103\bin\HostX64\x64\cl.exe]],
        {
            "/EHsc",
            "-nologo",
            "-W4",
            "-WX",
            "-Ox",
            "-fp:fast",
            "-std:c++20",
            "-Iinclude",
            [[-Ilib\imgui\include]],
            [[-Ilib\tiny_obj_loader\include]],
            [[-Ilib\vma\include]],
            "-DGLFW_INCLUDE_NONE",
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\f\fmt\8.1.1\3c4518782925487388553bdb3459c51d\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\g\glm\0.9.9+8\8335bab7c0314ab9921440de58961aac\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\Desktop\Documents\cmake_test\vcpkg\installed\x64-windows-static\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\g\glfw\3.3.6\0f1d7b80c6f84759bc07798549faa20b\include]],
            "-external:W0",
            [[-external:IC:\Users\KillByte\AppData\Local\.xmake\packages\v\vulkan-headers\1.2.198+0\06123fbd318a4a08a4b63d4c12e153b5\include]],
            "/Zc:preprocessor"
        }
    },
    files = {
        [[src\gui\camera_controller.cpp]]
    },
    depfiles_cl_json = "{\
    \"Version\": \"1.1\",\
    \"Data\": {\
        \"Source\": \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\src\\\\gui\\\\camera_controller.cpp\",\
        \"ProvidedModule\": \"\",\
        \"Includes\": [\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\gui\\\\camera_controller.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\core\\\\camera.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\glm.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\_fixes.hpp\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cmath\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\yvals.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\yvals_core.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\sal.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\concurrencysal.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vadefs.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xkeycheck.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\crtdbg.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime_new_debug.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime_new.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\crtdefs.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\use_ansi.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cstdlib\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\math.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_math.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\stdlib.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_malloc.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_search.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\stddef.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wstdlib.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\limits.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xtr1common\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\intrin0.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\intrin0.inl.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xutility\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\climits\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cstring\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\string.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_memory.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_memcpy_s.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\errno.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime_string.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wstring.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\utility\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\type_traits\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cstdint\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\stdint.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xstddef\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cstddef\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\initializer_list\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\concepts\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\compare\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\bit\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\isa_availability.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\limits\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cfloat\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\float.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cwchar\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cstdio\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\stdio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wstdio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_stdio_config.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\wchar.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wconio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wctype.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wdirect.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wio.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_share.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wprocess.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_wtime.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\sys\\\\stat.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\sys\\\\types.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cassert\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\assert.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\simd\\\\platform.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cassert\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\assert.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\fwd.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\qualifier.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\vec2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec2.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\compute_vector_relational.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int2_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\scalar_int_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint2_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\scalar_uint_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\vec3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec3.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int3_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint3_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\vec4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec4.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_bool4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_float4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_double4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_int4_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\vector_uint4_sized.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat2x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x2.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\matrix.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat2x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x3.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat2x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat2x4.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat3x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x2.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat3x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x3.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat3x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat3x4.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double3x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float3x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat4x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x2.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat4x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x3.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x3.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x3_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\mat4x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_mat4x4.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double4x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x4.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float4x4_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_matrix.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\geometric.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_geometric.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\exponential.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec1.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_vec1.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_exponential.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\vector_relational.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_vector_relational.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\_vectorize.hpp\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\cassert\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\assert.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\common.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\_fixes.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_common.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\compute_common.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_double2x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x2.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\ext\\\\matrix_float2x2_precision.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\trigonometric.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_trigonometric.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\packing.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_packing.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_half.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\setup.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\type_half.inl\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\integer.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glm\\\\0.9.9+8\\\\8335bab7c0314ab9921440de58961aac\\\\include\\\\glm\\\\detail\\\\func_integer.inl\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\intrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\setjmp.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\immintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\wmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\nmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\smmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\tmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\pmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\emmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\mmintrin.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\malloc.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\zmmintrin.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\ammintrin.h\",\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\core\\\\window.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\core\\\\input.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\g\\\\glfw\\\\3.3.6\\\\0f1d7b80c6f84759bc07798549faa20b\\\\include\\\\glfw\\\\glfw3.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vulkan.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vk_platform.h\",\
            \"c:\\\\users\\\\killbyte\\\\appdata\\\\local\\\\.xmake\\\\packages\\\\v\\\\vulkan-headers\\\\1.2.198+0\\\\06123fbd318a4a08a4b63d4c12e153b5\\\\include\\\\vulkan\\\\vulkan_core.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\string_view\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xstring\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\iosfwd\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xmemory\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\new\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\exception\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vcruntime_exception.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\eh.h\",\
            \"c:\\\\program files (x86)\\\\windows kits\\\\10\\\\include\\\\10.0.19041.0\\\\ucrt\\\\corecrt_terminate.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xatomic.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\tuple\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\xpolymorphic_allocator.h\",\
            \"c:\\\\program files (x86)\\\\microsoft visual studio\\\\2022\\\\buildtools\\\\vc\\\\tools\\\\msvc\\\\14.31.31103\\\\include\\\\vector\",\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\math\\\\coordinates.hpp\",\
            \"c:\\\\users\\\\killbyte\\\\desktop\\\\documents\\\\kazan\\\\include\\\\math\\\\constants.hpp\"\
        ],\
        \"ImportedModules\": [],\
        \"ImportedHeaderUnits\": []\
    }\
}"
}