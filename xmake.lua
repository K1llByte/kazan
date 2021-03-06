-------------- Package requirements --------------

add_requires("vulkan-headers")
add_requires("glm")
add_requires("glfw")
add_requires("fmt")
-- glslangValidator
add_requires("glslang", {configs = {binaryonly = true}})
add_requires("vcpkg::boost-pfr", {alias = "boost"})

---------------- Reused functions ----------------

function use_vulkan()
    if is_plat("windows") then 
        add_linkdirs("C:\\VulkanSDK\\1.3.216.0\\Lib")
        add_packages("vulkan-headers", {links = "vulkan-1"})
    else
        add_packages("vulkan-headers", {links = "vulkan"})
    end
end

function kazan_deps()
    -- Dependencies
    add_packages("glm")
    add_packages("fmt")
    add_packages("boost")
    add_packages("glfw")
    use_vulkan()
end

function compilation_settings()
    -- Common Compiler Options
    set_languages("cxx20") -- -std=c++20
    -- (xmake 2.6.8)
    --set_policy("build.warning", true) -- Allways show warnings
    set_warnings("allextra") -- -Wall -Wextra -Wfatal-errors (if error enabled)
    set_optimize("fastest") -- -O3

    --add_rules("mode.asan")
    -- add_rules("mode.lsan")
    -- add_rules("mode.ubsan")
    
    -- GCC flags
    if is_plat("linux", "macosx") then
        add_cxxflags("-Wshadow", "-Wpedantic")
    end
    -- GCC Options
    if is_plat("linux", "macosx") then
        add_links("pthread") --, "m", "dl")
    end
    -- MSVC Options
    if is_plat("windows") then 
        -- Allow variadic macros
        add_cxxflags("/Zc:preprocessor")
    end
    set_targetdir("bin/")
    -- LTO (xmake 2.6.9)
    --set_policy("build.optimization.lto")
    -- Include dirs
    add_includedirs("include")
    -- add_includedirs("lib/stb/include")
    add_includedirs("lib/tiny_obj_loader/include")
    add_includedirs("lib/imgui_docking/include")
    add_includedirs("lib/vma/include")
end

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

-- Target Kazan Static Library
target("kazan")
    -- Compiler Settings
    compilation_settings()
    -- Dependencies
    kazan_deps()
    -- Library
    set_kind("static")
    add_files("src/**.cpp")
    remove_files("src/gui/**.cpp")
    remove_files("src/main.cpp")
    --remove_files("src/core/shapes.*pp")

-------------------- Kazan GUI -------------------

-- Target ImGUI
target("imgui")
    set_kind("object")
    add_includedirs("lib/imgui_docking/include")
    add_packages("glfw")
    add_packages("vulkan-headers")
    add_files("lib/imgui_docking/src/**.cpp")

-- Target Demo using Kazan Lib
target("kazui")
    -- Compiler Settings
    compilation_settings()
    -- Dependencies
    kazan_deps()
    add_deps("imgui")
    add_deps("kazan")
    -- Binary
    set_kind("binary")
    add_files("src/gui/*.cpp")

------------------ Kazan Examples ----------------

-- "model_render/example4.cpp"
function make_examples(examples)
    for i,v in ipairs(examples) do
        target("example" .. tostring(i))
            -- Compiler Settings
            compilation_settings()
            -- Dependencies
            kazan_deps()
            add_deps("kazan")
            -- Binary
            set_kind("binary")
            add_files("examples/" .. tostring(v))
    end
end

make_examples({
    "triangle/example1.cpp",
    "triangle/example2.cpp",
    "triangle/example3.cpp",
    "model_render/example4.cpp"
})