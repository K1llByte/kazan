-- Package Requirements
add_requires("vulkan-headers")
add_requires("glm")
add_requires("glfw")
add_requires("fmt")

-------------------- Shaders ---------------------

-- Custom shader compilation rule
rule("glsl")
    set_extensions(".vert", ".frag")
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

-- Target Kazan Static Library
target("kazan")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    -- Compiler Options
    add_options("kb22")
    if is_plat("linux", "macosx") then
        add_links("pthread") --, "m", "dl")
    end
    add_includedirs("include")
    -- add_includedirs("include/lib/stb")
    add_includedirs("include/lib/imgui")
    -- add_includedirs("include/lib/imgui_docking")
    -- add_includedirs("include/lib/tiny_obj_loader")
    set_targetdir("bin/")
    -- Library
    set_kind("static")
    add_files("src/**.cpp")
    remove_files("src/gui/**.cpp")
    remove_files("src/main.cpp")
    -- Dependencies
    add_packages("vulkan-headers", {links = "vulkan"})
    add_packages("glfw")
    add_packages("glm")
    add_packages("fmt")

-------------------- Kazan GUI -------------------

-- Target Demo using Kazan Lib
target("demo")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    -- Compiler Options
    add_options("kb22")
    add_includedirs("include")
    -- add_includedirs("include/gui")
    -- Dependencies
    add_deps("kazan")
    add_packages("fmt")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("src/gui/*.cpp")

------------------ Kazan Examples ----------------

target("example1")
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra", "error") -- -Wall -Wextra -Wfatal-errors
    set_optimize("fastest") -- -O3
    -- FIXME: Not working
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    -- Compiler Options
    add_options("kb22")
    add_includedirs("include")
    add_deps("kazan")
    add_packages("fmt")
    set_targetdir("bin/")
    -- Binary
    set_kind("binary")
    add_files("examples/triangle/main.cpp")