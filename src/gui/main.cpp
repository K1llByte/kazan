#include "gui/console.hpp"
#include "utils/variant.hpp"

#include "utils/string.hpp"

void print_splited(const std::vector<std::string_view>& splited) {
    for(const auto& str : splited) {
        fmt::print("'{}'\n", std::string(str));
    }
    //for(size_t i = 0; i < splited.size; ++i) {
    //    fmt::print("'{}'\n", std::string(splited.data[i]));
    //}
}

void testing(const char* asd) {
    auto&& a = kzn::split(asd);
    fmt::print("size: {}\n", a.size());
    print_splited(std::move(a));

    //for(size_t i = 0; i < a.size; ++i) {
    //    fmt::print("'{}'\n", std::string(a.data[i]));
    //}
}

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
                    int radius = std::get<int>(args[0]);
                    float slices = std::get<float>(args[1]);
                    float stacks = std::get<float>(args[2]);
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
        //commands_tree.add_command(
        //    {"create", "cube"},
        //    [](Arg::Any* args) {
        //        fmt::print("called create cube\n");
        //    }
        //);
    }
    commands_tree.print();


    //print_splited(kzn::split((const char*) "debug"));
    //print_splited(kzn::split((const char*) "debug debug"));
    //kzn::console::testing("debug");
    fmt::print("====\n");
    //testing("debug debug");
    //commands_tree.execute("create sphere 1 20 20");
    commands_tree.execute("debug");


    //

    //{
    //    using kzn::console::Arg::Int;
    //    using kzn::console::Arg::Float;
    //    using kzn::console::Arg::String;
    //    commands_tree.add_command({"create","sphere"}, {Float("arg1"), Int("arg2")}, nullptr);
    //}

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