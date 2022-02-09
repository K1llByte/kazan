-- add_requires("vulkan-headers")
-- add_requires("vulkan-headers")
add_requires("vulkan-headers")

-- add_cxxflags("-lglfw", "-lvulkan", "-ldl", "-lpthread", "-lX11", "-lXxf86vm", "-lXrandr", "-lXi")
-- add_cxxflags("-std=c++2a", "-O2")
-- add_cxxflags("-Wall", "-Wextra", "-Wshadow", "-Wfatal-errors", "-Wpedantic")


-- add_shflags("xxx")

target("renderer")
    set_languages("cxx20")
    -- add_shflags("-lglfw", "-lvulkan", "-ldl", "-lpthread", "-lX11", "-lXxf86vm", "-lXrandr", "-lXi")
    -- add_shflags("-lvulkan")
    -- add_mxxflags("-std=c++2a", "-O2")
    -- add_mxxflags("-Wall", "-Wextra", "-Wshadow", "-Wfatal-errors", "-Wpedantic")
    add_ldflags("-lglfw", "-ldl", "-lpthread", "-lX11", "-lXxf86vm", "-lXrandr", "-lXi")
    set_warnings("allextra")
    set_optimize("fastest")

    add_includedirs("include")
    add_includedirs("include/lib/imgui")
    add_includedirs("include/lib/pegtl")
    add_includedirs("include/lib/tiny_obj_loader")
    set_kind("binary")
    add_files("src/**.cpp")
    add_packages("vulkan-headers", {links = "vulkan"})
