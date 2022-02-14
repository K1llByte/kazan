-- Package Requirements
add_requires("vulkan-headers")
add_requires("glm")
add_requires("glfw")

-------------------- Shaders ---------------------

-- Custom shader compilation rule
rule("glsl")
    set_extensions(".vert", ".frag")
    on_build_file(function (target, sourcefile, opt)
        import("core.project.depend")
        import("utils.progress") -- it only for v2.5.9, we need use print to show progress below v2.5.8

        -- make sure build directory exists
        os.mkdir(target:targetdir())

        -- replace .md with .html
        local targetfile = path.join(target:targetdir(), sourcefile .. ".html")

        -- only rebuild the file if its changed since last run
        depend.on_changed(function ()
            -- call pandoc to make a standalone html file from a markdown file
            os.vrunv('glslangValidator', {"-V", sourcefile, "-o", targetfile})
            progress.show(opt.progress, "${color.build.object}shader %s", sourcefile)
        end, {files = sourcefile})
    end)


-- Target Shaders Compilation
target("shaders")
    set_kind("object")

    -- make the test target support the construction rules of the spir-v files
    add_rules("glsl")

    -- adding shader files to build
    add_files("shaders/**.vert")
    add_files("shaders/**.frag")

-------------------- Renderer --------------------

-- Target Renderer Executable
target("renderer")
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
    -- Executable
    set_kind("binary")
    add_files("src/**.cpp")
    -- Dependencies
    add_packages("vulkan-headers", {links = "vulkan"})
    add_packages("glfw")
    add_packages("glm")