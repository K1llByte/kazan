#include "gui/console.hpp"

#include <fmt/core.h>

#include "utils/string.hpp"

#include <GLFW/glfw3.h>



// int main() {
//     using kzn::console::Commands;
//     using kzn::console::Arg;

//     Commands commands_tree;

//     {
//         using kzn::console::Int;
//         using kzn::console::Float;
//         using kzn::console::String;
//         commands_tree.add_command(
//             {"create", "sphere"},
//             {Float("radius"), Int("slices"), Int("stacks")},
//             [](Arg::Any* args) {
//                 float radius = std::get<float>(args[0]);
//                 int slices = std::get<int>(args[1]);
//                 int stacks = std::get<int>(args[2]);
//                 fmt::print("called create sphere {} {} {}\n", radius, slices, stacks);
//             }
//         );
//         commands_tree.add_command(
//             {"debug"},
//             [](Arg::Any*) {
//                 fmt::print("Debug command\n");
//             }
//         );
//     }

//     auto err = commands_tree.execute("create sphere 1 20 20");
//     fmt::print("{}\n", kzn::console::error_message(err));
// }

#include <type_traits>
#include "vk/uniform.hpp"

// struct Bar;

using namespace kzn;

struct Foo {
    glsl::vec3 a;
    glsl::vec3 b;
    
};

int main() {
    fmt::print("is uniform? {}\n", glsl::is_uniform<Foo>());
}