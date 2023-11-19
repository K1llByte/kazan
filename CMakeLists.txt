cmake_minimum_required(VERSION 3.20)
project(Kazan VERSION 1.0 LANGUAGES CXX)

#################################### Options ####################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##################################### Paths #####################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

################################# Dependencies ##################################
include(FetchContent)

# - Vulkan
# TODO: Automatically install Vulkan SDK
find_package(Vulkan REQUIRED)

# - Fmt
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt)
FetchContent_MakeAvailable(fmt)

# - Glfw
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_MakeAvailable(glfw)

# - ImGui (with docking)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG "origin/docking")
FetchContent_MakeAvailable(imgui)
add_library(CustomImgui STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(CustomImgui PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(CustomImgui PRIVATE glfw ${Vulkan_LIBRARIES})

# - Glm
FetchContent_Declare(glm 
    GIT_REPOSITORY https://github.com/g-truc/glm)
FetchContent_MakeAvailable(glm)

# - Entt
FetchContent_Declare(entt 
    GIT_REPOSITORY https://github.com/skypjack/entt)
FetchContent_MakeAvailable(entt)

# - VulkanMemoryAllocator
FetchContent_Declare(vma 
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
FetchContent_MakeAvailable(vma)

################################## Main Target ##################################
file(GLOB KAZAN_SOURCES
    "src/core/**.cpp"
    "src/ecs/**.cpp"
    "src/graphics/**.cpp"
    "src/vk/**.cpp"
)
set(KAZAN_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(KazanLib STATIC ${KAZAN_SOURCES})
target_link_libraries(KazanLib PUBLIC
    fmt
    glfw
    glm
    EnTT::EnTT
    GPUOpen::VulkanMemoryAllocator
)
target_include_directories(KazanLib PUBLIC ${KAZAN_INCLUDE_PATH})
target_compile_definitions(KazanLib
    PUBLIC
      $<$<CONFIG:Debug>:DEBUG>
      $<$<CONFIG:RelWithDebInfo>:DEBUG>
      $<$<CONFIG:Release>:RELEASE>
      $<$<CONFIG:MinSizeRel>:RELEASE>
)

add_executable(KazanEditor "src/editor/editor_app.cpp")
target_link_libraries(KazanEditor PUBLIC KazanLib CustomImgui)
target_compile_definitions(KazanEditor
    PUBLIC
      $<$<CONFIG:Debug>:DEBUG>
      $<$<CONFIG:RelWithDebInfo>:DEBUG>
      $<$<CONFIG:Release>:RELEASE>
      $<$<CONFIG:MinSizeRel>:RELEASE>
)

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
    set_property(TARGET Hello PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()