#include "kazan.hpp"

using namespace kzn;

int main()
{
    Log::info("Hello World!");
    
    auto instance = vk::InstanceBuilder()
        .enable_validation_layers()
        .build();
}