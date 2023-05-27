#include "core/log.hpp"
#include "vk/device.hpp"
#include "vk/instance.hpp"

int main() {
    using namespace kzn;

    auto instance = vk::Instance({
        .with_validation = true,
    });



    auto device = vk::Device({
         .instance = instance,
    });
}