#include "core/log.hpp"
#include "core/window.hpp"
#include "vk/device.hpp"
#include "vk/instance.hpp"
#include "vk/surface.hpp"
#include "vk/swapchain.hpp"
#include "vk/pipeline.hpp"
#include "vk/render_pass.hpp"
#include "vk/cmd_buffer.hpp"

// #include "vk/vulkan_context.hpp"

// TODO: Create alias type in core/ Ref = std::reference_wrapper
// and change every non_owning ptr to Ref 

using namespace kzn;

std::vector<vk::Framebuffer> create_swapchain_framebuffers(
    vk::RenderPass& render_pass,
    vk::Swapchain& swapchain)
{
    std::vector<vk::Framebuffer> swapchain_framebuffers;
    const size_t imgs_count = swapchain.image_views().size();
    swapchain_framebuffers.reserve(imgs_count);

    for(size_t i = 0; i < imgs_count; ++i) {
        swapchain_framebuffers.emplace_back(
            render_pass,
            std::vector{ swapchain.image_views()[i] },
            swapchain.extent()
        );
    }

    return swapchain_framebuffers;
}

vk::RenderPass simple_pass(vk::Device& device, VkFormat color_format) {
    std::vector attachment_descs {
        vk::AttachmentDesc{
            .format = color_format,
            .load_op = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .store_op = VK_ATTACHMENT_STORE_OP_STORE,
            .final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
    };
    auto color_ref = vk::AttachmentRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    return vk::RenderPass(
        device,
        vk::RenderPassParams{
            .attachments = attachment_descs,
            .subpasses = {
                vk::SubpassDesc{
                    .color_attachments = { color_ref },
                }
            },
        }
    );
}

vk::Pipeline triangle_pipeline(vk::RenderPass& render_pass) {
    return vk::Pipeline(
        render_pass.device(),
        vk::PipelineStages{
            .vertex = "assets/shaders/triangle/triangle.vert.spv",
            .fragment = "assets/shaders/triangle/triangle.frag.spv",
        },
        vk::PipelineConfig(render_pass)
    );
}

int main() try {
    auto window = Window("Kazan Engine", 1000, 800);

    auto instance = vk::Instance({
        .extensions = window.required_extensions(),
        .with_validation = true,
    });
    auto surface = window.create_surface(instance);
    auto device = vk::Device(instance, {
        .extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        .surface = surface.vk_surface()
    });
    auto swapchain = vk::Swapchain(device, surface, window.extent());

    auto render_pass = simple_pass(device, swapchain.image_format());
    auto pipeline = triangle_pipeline(render_pass);
    auto framebuffers = create_swapchain_framebuffers(render_pass, swapchain);

    while(!window.is_closed()) {
        window.poll_events();
    }
}
catch(vk::ResultError re) {
    Log::error("{}", re.message());
}