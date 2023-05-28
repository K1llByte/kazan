#include "core/log.hpp"
#include "core/window.hpp"
#include "vk/device.hpp"
#include "vk/instance.hpp"
#include "vk/surface.hpp"

// TODO: Create alias type in core/ Ref = std::reference_wrapper
// and change every non_owning ptr to Ref 

using namespace kzn;
int main() try {

    auto window = Window("Kazan Engine", 1000, 800);

    auto instance = vk::Instance({
        .extensions = window.required_extensions(),
        .with_validation = true,
    });
    auto device = vk::Device(instance);
    auto surface = window.create_surface(instance);

    while(!window.is_closed()) {
        window.poll_events();
    }
}
catch(vk::ResultError re) {
    Log::error("{}", re.message());
}