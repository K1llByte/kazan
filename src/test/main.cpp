#include "core/app.hpp"
#include "core/basic_app.hpp"
#include "editor/editor_system.hpp"
#include "graphics/render_system.hpp"
#include "graphics/renderer.hpp"
#include "graphics/stages/debug_stage.hpp"
#include "graphics/stages/gpu_planet_stage.hpp"
#include "graphics/stages/render_stage.hpp"
#include "resources/resources.hpp"
#include "vk/pipeline_builder.hpp"
#include "vk/shader_code.hpp"

#include <spirv_reflect.h>

using namespace kzn;

inline void create_test_level(Scene& scene) {
    auto entity = scene.registry.create();
    entity.emplace<Transform2DComponent>();

    auto camera = scene.registry.create();
    camera.add(Camera2DComponent{
        .use_viewport_aspect_ratio = true,
    });
}

inline void init_render_stages(RenderSystem& render_sys) {
    render_sys.emplace_stage<SpriteStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera_dset()
    );
    render_sys.emplace_stage<GpuPlanetStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera_dset()
    );
    render_sys.emplace_stage<DebugStage>(
        render_sys.context<Renderer>(),
        render_sys.screen_render_pass(),
        render_sys.camera_dset()
    );
    // NOTE: EditorSystem will inject ImguiStage by event
}

inline void test_spirv_reflect() {
    auto shader_ptr = g_resources.load<vk::ShaderCode>("shaders://debug.vert.spv");

    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    auto result = spvReflectCreateShaderModule(shader_ptr->bytecode.size(), shader_ptr->bytecode.data(), &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    Log::debug("module.descriptor_set_count: {}", module.descriptor_set_count);
    for(size_t i = 0; i < module.descriptor_set_count; ++i) {
        Log::debug("- set: {}, bindings: {}", i, module.descriptor_sets[i].binding_count);
    }
    Log::debug("module.push_constant_block_count: {}", module.push_constant_block_count);
    for(size_t i = 0; i < module.push_constant_block_count; ++i) {
        Log::debug("- name: {}", module.push_constant_blocks[i].name);
        Log::debug("- size: {}", module.push_constant_blocks[i].size);
        Log::debug("- offset: {}", module.push_constant_blocks[i].offset);
    }

    // ///////////////////////////////////////////////////////////////////////////
    // // Inputs
    // ///////////////////////////////////////////////////////////////////////////

    // // Enumerate and extract shader's input variables
    // uint32_t input_count = 0;
    // result = spvReflectEnumerateInputVariables(&module, &input_count, nullptr);
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);
    // auto input_vars = std::make_unique<SpvReflectInterfaceVariable*[]>(input_count);
    // result = spvReflectEnumerateInputVariables(&module, &input_count, input_vars.get());
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Log::debug("Input count: {}", input_count);
    // for(size_t i = 0; i < input_count; ++i) {
    //     Log::debug("- name: {}", input_vars[i]->name);
    //     Log::debug("- type: {}", input_vars[i]->type_description->id);
    // }

    // ///////////////////////////////////////////////////////////////////////////
    // // Outputs
    // ///////////////////////////////////////////////////////////////////////////

    // // Enumerate and extract shader's output variables
    // uint32_t output_count = 0;
    // result = spvReflectEnumerateOutputVariables(&module, &output_count, nullptr);
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);
    // auto output_vars = std::make_unique<SpvReflectInterfaceVariable*[]>(output_count);
    // result = spvReflectEnumerateOutputVariables(&module, &output_count, output_vars.get());
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Log::debug("Output count: {}", output_count);
    // for(size_t i = 0; i < output_count; ++i) {
    //     Log::debug("- name: {}", output_vars[i]->name);
    //     Log::debug("- type: {}", output_vars[i]->type_description->id);
    // }

    // ///////////////////////////////////////////////////////////////////////////
    // // Descriptor Sets
    // ///////////////////////////////////////////////////////////////////////////

    // // Enumerate and extract shader's descriptor sets
    // uint32_t dsets_count = 0;
    // result = spvReflectEnumerateDescriptorSets(&module, &dsets_count, nullptr);
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);
    // auto dsets = std::make_unique<SpvReflectDescriptorSet*[]>(dsets_count);
    // result = spvReflectEnumerateDescriptorSets(&module, &dsets_count, dsets.get());
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Log::debug("Push constants count: {}", dsets_count);
    // for(size_t i = 0; i < dsets_count; ++i) {
    //     Log::debug("- set: {}", dsets[i]->set);
    //     for(size_t b = 0; b < dsets[i]->binding_count; ++b) {
    //         Log::debug("  - binding: {}", b);
    //         Log::debug("  - name: {}", dsets[i]->bindings[b]->name);
    //         Log::debug("  - type: {}", dsets[i]->bindings[b]->type_description->id);

    //     }
    // }

    // ///////////////////////////////////////////////////////////////////////////
    // // Push Constants
    // ///////////////////////////////////////////////////////////////////////////

    // // Enumerate and extract shader's push constants variables
    // uint32_t pconstants_count = 0;
    // result = spvReflectEnumeratePushConstantBlocks(&module, &pconstants_count, nullptr);
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);
    // auto pconstants_vars = std::make_unique<SpvReflectBlockVariable*[]>(pconstants_count);
    // result = spvReflectEnumeratePushConstantBlocks(&module, &pconstants_count, pconstants_vars.get());
    // assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Log::debug("Push constants count: {}", pconstants_count);
    // for(size_t i = 0; i < pconstants_count; ++i) {
    //     Log::debug("- name: {}", pconstants_vars[i]->name);
    // }

    // Destroy the reflection data when no longer required.
    spvReflectDestroyShaderModule(&module);
}

struct TestApp : public BasicApp {
    TestApp() {
        // Testing spirv-reflect
        test_spirv_reflect();
        vk::PipelineBuilder{}
            .set_vertex_stage(load_shader("shaders://debug.vert.spv"));
        
        // Initialize systems
        auto& render_sys = m_systems.emplace<RenderSystem>();
        init_render_stages(render_sys);
        // EditorSystem auto registers as dependency before RenderSystem
        m_systems.emplace<EditorSystem>(m_window, m_input, m_console);

        // Load level entities
        create_test_level(m_scene);
    }
};

KZN_CREATE_APP(TestApp)