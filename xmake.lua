-- Package Requirements
add_requires("vulkan-headers")
add_requires("glm")
add_requires("glfw")
add_requires("fmt")
add_requires("glslang", {configs = {binaryonly = true}})

add_requires("vcpkg::boost-pfr", {alias = "boost"})

-------------------- Shaders ---------------------

-- Custom shader compilation rule
rule("glsl")
    set_extensions(".vert", ".comp", ".frag")
    on_build_file(function (target, sourcefile, opt)
        import("core.project.depend")
        import("utils.progress") -- it only for v2.5.9, we need use print to show progress below v2.5.8

        -- make sure build directory exists
        os.mkdir(target:targetdir())

        -- append target file with .spv
        -- local targetfile = path.join(target:targetdir(), sourcefile .. ".spv")
        local targetfile =  sourcefile .. ".spv"

        -- only rebuild the file if its changed since last run
        depend.on_changed(function ()
            -- call glslangValidator to compile glsl to spv
            os.vrunv('glslangValidator', {"-V", sourcefile, "-o", targetfile})
            progress.show(opt.progress, "${color.build.object}shader %s to %s", sourcefile, targetfile)
        end, {files = sourcefile})
    end)

-- Target Shaders Compilation
target("shaders")
    set_kind("object")
    -- make the test target support the construction rules of the spir-v files
    add_rules("glsl")
    set_targetdir("./")
    -- adding shader files to build
    add_files("assets/shaders/**.vert")
    add_files("assets/shaders/**.frag")

-------------------- Kazan Lib -------------------

-- target("foo")
--     set_kind("headeronly")
--     add_headerfiles("src/foo.h")

-- Target Kazan Static Library
target("kazan")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra") -- -Wall -Wextra -Wfatal-errors (if error enabled)
    set_optimize("fastest") -- -O3
    if is_plat("linux", "macosx") then
        add_cxxflags("-Wshadow", "-Wpedantic")
    end
    set_targetdir("bin/")
    -- Compiler Options
    add_options("kb22")
    if is_plat("linux", "macosx") then
        add_links("pthread") --, "m", "dl")
    end
    add_cxxflags("/Zc:preprocessor")
    add_includedirs("include")
    -- add_includedirs("lib/include/imgui")
    -- add_includedirs("lib/imgui_docking/include")
    -- add_includedirs("lib/stb/include")
    add_includedirs("lib/tiny_obj_loader/include")
    add_includedirs("lib/vma/include")
    -- Dependencies
    add_packages("vulkan-headers")
    add_packages("glfw")
    add_packages("glm")
    add_packages("fmt")
    add_packages("boost")
    -- Library
    set_kind("static")
    add_files("src/**.cpp")
    remove_files("src/gui/**.cpp")
    remove_files("src/main.cpp")
    remove_files("src/core/shapes.*pp")

-------------------- Kazan GUI -------------------

target("imgui")
    set_kind("object")
    add_includedirs("lib/imgui/include")
    add_packages("glfw")
    add_packages("vulkan-headers")
    add_links("vulkan")
    -- add_headerfiles("lib/imgui/include/*.h")
    add_files("lib/imgui/src/**.cpp")

-- Target Demo using Kazan Lib
target("kazui")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    set_targetdir("bin/")
    -- GCC flags
    if is_plat("linux", "macosx") then
        add_cxxflags("-Wshadow", "-Wpedantic")
    end
    -- MSVC flags
    --if is_plat("windows") then 
    add_cxxflags("/Zc:preprocessor")
    --end
    -- Compiler Options
    add_options("kb22")
    add_includedirs("include")
    add_includedirs("lib/imgui/include")
    add_includedirs("lib/tiny_obj_loader/include")
    add_includedirs("lib/vma/include")
    -- Dependencies
    add_deps("imgui")
    add_deps("kazan")
    add_packages("fmt")
    add_packages("glm")
    add_packages("boost")
    add_packages("glfw")
    add_linkdirs("C:\\VulkanSDK\\1.3.216.0\\Lib")
    add_packages("vulkan-headers", {links = "vulkan-1"})
    -- Binary
    set_kind("binary")
    add_files("src/gui/*.cpp")

------------------ Kazan Examples ----------------

-- TODO: Every example uses the same configuration,
-- figure out how to reuse and minimize lines

target("example1")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wpedantic")
    -- Compiler Options
    add_includedirs("include")
    add_includedirs("lib/vma/include")
    add_deps("kazan")
    add_packages("fmt")
    add_packages("glm")
    add_packages("boost")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("examples/triangle/example1.cpp")

target("example2")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wpedantic")
    -- Compiler Options
    add_includedirs("include")
    add_includedirs("lib/vma/include")
    add_deps("kazan")
    add_packages("fmt")
    add_packages("glm")
    add_packages("boost")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("examples/triangle/example2.cpp")

target("example3")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wpedantic")
    -- Compiler Options
    add_includedirs("include")
    add_includedirs("lib/vma/include")
    add_deps("kazan")
    add_packages("fmt")
    add_packages("glm")
    add_packages("boost")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("examples/triangle/example3.cpp")

target("example4")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wpedantic")
    -- Compiler Options
    add_includedirs("include")
    add_includedirs("lib/vma/include")
    add_deps("kazan")
    add_packages("fmt")
    add_packages("glm")
    add_packages("boost")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("examples/model_render/example4.cpp")