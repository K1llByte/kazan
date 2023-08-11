#include "core/log.hpp"
#include "core/window.hpp"
#include "vk/device.hpp"
#include "vk/instance.hpp"
#include "vk/surface.hpp"
#include "vk/swapchain.hpp"
#include "vk/pipeline.hpp"

// #include "vk/vulkan_context.hpp"

// TODO: Create alias type in core/ Ref = std::reference_wrapper
// and change every non_owning ptr to Ref 

using namespace kzn;

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

    //vk::Pipeline()

    while(!window.is_closed()) {
        window.poll_events();
    }
}
catch(vk::ResultError re) {
    Log::error("{}", re.message());
}