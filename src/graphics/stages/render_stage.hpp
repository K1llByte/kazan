#pragma once

#include "ecs/scene.hpp"
#include "graphics/texture.hpp"
#include "resources/resources.hpp"
#include "vk/cmd_buffer.hpp"
#include "vk/shader_code.hpp"

#include <memory>

namespace kzn {

// FIXME: Temporary util function
inline std::shared_ptr<vk::ShaderCode> load_shader(const std::string_view path) {
    return g_resources.load<vk::ShaderCode>(path);
}
inline std::shared_ptr<Texture> load_texture(const std::string_view path) {
    return g_resources.load<Texture>(path);
}

struct RenderStage {
    virtual ~RenderStage() {}
    virtual void pre_render(Scene& scene) {}
    virtual void render(Scene& scene, vk::CommandBuffer& cmd_buffer) = 0;
    virtual void post_render(Scene& scene) {}
};

} // namespace kzn