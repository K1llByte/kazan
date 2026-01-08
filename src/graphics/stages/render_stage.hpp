#pragma once

#include "vk/cmd_buffer.hpp"

namespace kzn {

struct RenderStage {
    virtual void pre_render() {}
    virtual void render(vk::CommandBuffer& cmd_buffer) = 0;
    virtual void post_render() {}
};

} // namespace kzn