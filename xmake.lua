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
    -- Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra") -- -Wall -Wextra
    set_optimize("fastest") -- -O3
    set_targetdir("bin/")
    add_links("pthread") -- -lpthread
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    add_includedirs("include")
    add_includedirs("include/lib/stb")
    add_includedirs("include/lib/imgui")
    add_includedirs("include/lib/pegtl")
    add_includedirs("include/lib/tiny_obj_loader")
    -- Library
    set_kind("static")
    add_files("src/**.cpp")
    remove_files("src/gui/**.cpp")
    -- Dependencies
    add_packages("vulkan-headers", {links = "vulkan"})
    add_packages("glfw")
    add_packages("glm")

-------------------- Kazan GUI -------------------

-- Target Demo using Kazan Lib
target("demo")
    -- Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra") -- -Wall -Wextra
    set_optimize("fastest") -- -O3
    set_targetdir("bin/")
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    add_includedirs("include") -- Only kazan.hpp is allowed to be included
    -- add_includedirs("include/gui")
    -- add_includedirs("include/lib/stb")
    -- add_includedirs("include/lib/imgui")
    -- add_includedirs("include/lib/tiny_obj_loader")
    -- Dependencies
    add_deps("kazan")
    -- add_packages("glm")
    add_packages("fmt")
    -- Library
    set_kind("binary")
    add_files("src/gui/*.cpp")
    -- add_files("src/gui/**/*.cpp")