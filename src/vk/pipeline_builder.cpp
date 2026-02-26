#include "pipeline_builder.hpp"

namespace kzn::vk {

PipelineBuilder& PipelineBuilder::set_vertex_stage(std::shared_ptr<ShaderCode> shader) {
    m_vertex_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_tess_control_stage(std::shared_ptr<ShaderCode> shader) {
    m_tess_control_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_tess_evaluation_stage(std::shared_ptr<ShaderCode> shader) {
    m_tess_evaluation_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_geometry_stage(std::shared_ptr<ShaderCode> shader) {
    m_geometry_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_fragment_stage(std::shared_ptr<ShaderCode> shader) {
    m_fragment_stage = std::move(shader);
    return *this;
}

PipelineBuilder& PipelineBuilder::set_topology(VkPrimitiveTopology topology) {
    m_input_assembly_info.topology = topology;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_polygon_mode(VkPolygonMode polygon_mode) {
    m_rasterization_info.polygonMode = polygon_mode;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_front_face(VkFrontFace font_face) {
    m_rasterization_info.frontFace = font_face;
    return *this;
}

PipelineBuilder& PipelineBuilder::set_line_width(float line_width) {
    m_rasterization_info.lineWidth = line_width;
    return *this;
}


// Pipeline PipelineBuilder::build() {
//     // TODO: new Pipeline ctor that will accept 
//     return 
// }

} // namespace kzn::vk
