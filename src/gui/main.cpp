#include "gui/console.hpp"
#include "utils/variant.hpp"

int main() {
    using kzn::console::Commands;
    using kzn::console::Arg;

    Commands commands_tree;
    commands_tree.add_command({"create", "sphere"}, { Arg{"arg1", Arg::FLOAT}, Arg{"arg2", Arg::INT} }, nullptr);
    commands_tree.add_command({"create", "cube"}, {}, nullptr);
    commands_tree.print();

    //commands_tree.add_command(
    //    "create",
    //    "sphere",
    //    {Arg("arg1", Arg::STRING), Arg("arg2", Arg::FLOAT)},
    //    [](auto arg1, auto arg2) {
    //        log::debug("Hello {} {}", arg1, arg2);    
    //    }
    //    );
    //);   
}