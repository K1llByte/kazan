-- Package Requirements
add_requires("vulkan-headers")
add_requires("glfw")

target("renderer")
    -- Compiler Options
    set_languages("cxx20") -- -std=c++20
    set_warnings("allextra") -- -Wall -Wextra
    set_optimize("fastest") -- -O3
    set_targetdir("bin/")
    add_links("pthread") -- -lpthread
    add_cxxflags("-Wshadow", "-Wfatal-errors", "-Wpedantic")
    add_includedirs("include")
    add_includedirs("include/lib/imgui")
    add_includedirs("include/lib/pegtl")
    add_includedirs("include/lib/tiny_obj_loader")
    -- Executable
    set_kind("binary")
    add_files("src/**.cpp")
    -- Dependencies
    add_packages("vulkan-headers", {links = "vulkan"})
    add_packages("glfw")

