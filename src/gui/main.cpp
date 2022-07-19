#include "gui/console.hpp"

#include <fmt/core.h>

#include "utils/string.hpp"

int main() {
    using kzn::console::Commands;
    using kzn::console::Arg;

    Commands commands_tree;

    int global_coiso = 0;
    {
        using kzn::console::Int;
        using kzn::console::Float;
        using kzn::console::String;
        commands_tree.add_command(
            {"create", "sphere"},
            {Float("radius"), Int("slices"), Int("stacks")},
            [&global_coiso](Arg::Any* args) {
                //auto [r,sl,st] = args;
                int radius = std::get<float>(args[0]);
                int slices = std::get<int>(args[1]);
                int stacks = std::get<int>(args[2]);
                fmt::print("called create sphere {} {} {}\n", radius, slices, stacks);
                ++global_coiso;
            }
        );
        commands_tree.add_command(
            {"debug"},
            [](Arg::Any*) {
                fmt::print("Debug command\n");
            }
        );
    }

    auto err = commands_tree.execute("create sphere 1 20 20");
    fmt::print("{}\n", kzn::console::error_message(err));

    //err = commands_tree.execute("debug");
    //fmt::print("{}\n", err_msgs[err]);
}