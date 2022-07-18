#include "gui/console.hpp"
#include "utils/variant.hpp"

#include "utils/string.hpp"

int main() {
    using kzn::console::Commands;
    using kzn::console::Arg;

    Commands commands_tree;

    {
        using kzn::console::Int;
        using kzn::console::Float;
        using kzn::console::String;
        commands_tree.add_command(
            {"create", "sphere"},
            {Float("radius"), Int("slices"), Int("stacks")},
            [](Arg::Any* args) {
                try {
                    //auto [radius, slices, stacks] = fetch_args<int,float,float>(args);
                    
                    int radius = std::get<float>(args[0]);
                    float slices = std::get<int>(args[1]);
                    float stacks = std::get<int>(args[2]);
                    fmt::print("called create sphere {} {} {}\n", radius, slices, stacks);
                }
                catch(std::bad_variant_access e) {
                    fmt::print("ERROR: Bad variant access\n");
                }
            }
        );
        commands_tree.add_command(
            {"debug"},
            [](Arg::Any*) {
                fmt::print("Debug command\n");
            }
        );
    }


    commands_tree.execute("create sphere 1 20 20");
    commands_tree.execute("debug");
}