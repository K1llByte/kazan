#pragma once

#include "vk/pipeline.hpp"

namespace kzn {
    inline vk::Pipeline create_triangle_pipeline(vk::RenderPass& _render_pass) {
        // NOTE: Temporary
        // auto cache = DescriptorSetLayoutCache(&Context::device());
        // auto allocator = DescriptorSetAllocator(&Context::device());

        // auto desc_set_0_layout = vk::DescriptorSetLayoutBuilder()
        //     .add_uniform(0)
        //     .build(Context::device(), layout_cache);

        auto pipeline_layout = vk::PipelineLayoutBuilder(&Context::device())
            .build();
        
        auto pipeline_config = vk::PipelineConfigBuilder(pipeline_layout, _render_pass)
            .set_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN)
            .set_polygon_mode(VK_POLYGON_MODE_FILL)
            .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
            .set_front_face(VK_FRONT_FACE_CLOCKWISE)
            .build();

        return vk::Pipeline(
            &Context::device(),
            "assets/shaders/triangle/triangle.vert.spv",
            "assets/shaders/triangle/triangle.frag.spv",
            pipeline_config
        );
    }

    // inline vk::Pipeline create_phong_pipeline(vk::RenderPass& _render_pass) {
    //     auto pipeline_layout = vk::PipelineLayoutBuilder(&Context::device())
    //             .add_push_constant(sizeof(PVM), VK_SHADER_STAGE_ALL_GRAPHICS) 
    //             .add_descriptor_set_layout(desc_set0_layout)
    //         .build();
        
    //     auto pipeline_config = vk::PipelineConfigBuilder(pipeline_layout, _render_pass)
    //         .set_polygon_mode(VK_POLYGON_MODE_FILL)
    //         .set_dynamic_states({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR})
    //         .set_type_vtx_input<Vertex>()
    //         .build();

    //     return vk::Pipeline(
    //         &Context::device(),
    //         "assets/shaders/mesh/mesh.vert.spv",
    //         "assets/shaders/mesh/mesh.frag.spv",
    //         pipeline_config
    //     );
    // }
} // namespace kzn