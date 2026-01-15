cmake_minimum_required(VERSION 3.20)
project(Kazan VERSION 1.0 LANGUAGES CXX)

#################################### Options ####################################

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_SYSTEM_INCLUDE_PATH}")

##################################### Paths #####################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

################################# Dependencies ##################################
include(FetchContent)

# - Vulkan
find_package(Vulkan REQUIRED)
# - X11 - On wayland might be required to force to use XWayland
find_package(X11 REQUIRED)
# - Fmt
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)
# - Glfw
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)
# - ImGui (with docking)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    # Latest docking branch commit
    GIT_TAG 01a4cff8f9190b7f3731e4d1cef818ca675e9a42
)
FetchContent_MakeAvailable(imgui)
# - Glm
FetchContent_Declare(glm 
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)
# - Boost-pfr
FetchContent_Declare(boostpfr
    GIT_REPOSITORY https://github.com/boostorg/pfr
    GIT_TAG boost-1.90.0
)
FetchContent_MakeAvailable(boostpfr)
# - Stb
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb
    # Latest main branch commit
    GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296
)
FetchContent_MakeAvailable(stb)
# - Entt
FetchContent_Declare(entt 
    GIT_REPOSITORY https://github.com/skypjack/entt
    GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(entt)
# - VulkanMemoryAllocator
FetchContent_Declare(vma 
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG v3.3.0
)
FetchContent_MakeAvailable(vma)
# - Box2D
FetchContent_Declare(box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d
    GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(box2d)

################################## Kazan Target #################################

# Kazan Library
file(GLOB KAZAN_SOURCES
    "src/editor/**.cpp"
    "src/core/**.cpp"
    "src/ecs/**.cpp"
    "src/graphics/**.cpp"
    "src/vk/**.cpp"
)
# Add ImGui backends
add_library(KazanLib STATIC ${KAZAN_SOURCES})
target_sources(KazanLib PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_link_libraries(KazanLib PRIVATE X11::X11)
target_link_libraries(KazanLib PUBLIC
    fmt
    glfw
    glm
    EnTT::EnTT
    boost_pfr
    # boost_type_traits
    GPUOpen::VulkanMemoryAllocator
    box2d
    ${Vulkan_LIBRARIES}
)
set(KAZAN_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src ${stb_SOURCE_DIR} ${imgui_SOURCE_DIR})
target_include_directories(KazanLib PUBLIC ${KAZAN_INCLUDE_PATH})
target_include_directories(KazanLib SYSTEM PUBLIC boost_pfr)
target_compile_definitions(KazanLib
    PUBLIC
      $<$<CONFIG:Debug>:DEBUG>
      $<$<CONFIG:RelWithDebInfo>:DEBUG>
      $<$<CONFIG:Release>:RELEASE>
      $<$<CONFIG:MinSizeRel>:RELEASE>
)
set_target_properties(KazanLib PROPERTIES
    OUTPUT_NAME "kazan"
)

################################## Test Target ##################################

add_executable(TestBin "src/test/main.cpp")
target_link_libraries(TestBin PRIVATE KazanLib)

target_include_directories(TestBin PUBLIC ${KAZAN_INCLUDE_PATH})
target_compile_definitions(TestBin
    PUBLIC
      $<$<CONFIG:Debug>:DEBUG>
      $<$<CONFIG:RelWithDebInfo>:DEBUG>
      $<$<CONFIG:Release>:RELEASE>
      $<$<CONFIG:MinSizeRel>:RELEASE>
)
set_target_properties(TestBin PROPERTIES
    OUTPUT_NAME "kazantest"
)

################################# Clang Options #################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nullability-completeness")
endif()

################################## Mold Linker ##################################

find_program(MOLD_EXECUTABLE "mold")

if(MOLD_EXECUTABLE)
    message(STATUS "Found mold linker: ${MOLD_EXECUTABLE}")
    # TODO: Missing support for msvc
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
endif()

############################ Link-Time-Optimization #############################

include(CheckIPOSupported)

check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT USE_LTO_ERROR)

if(LTO_SUPPORTED)
    message(STATUS "Enabled Link Time Optimization (LTO)")
    set_property(TARGET KazanLib PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()