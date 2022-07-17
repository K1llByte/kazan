#include "gui/console.hpp"

#include <stdexcept>
#include <algorithm>
#include <cstring>
#include "utils/variant.hpp"

namespace kzn::console {
    void Commands::add_command(
        std::initializer_list<const char*> cmd_path,
        std::initializer_list<Arg> arguments,
        void* action_function)
    {
        CmdTree* current_tree = &command_tree;
        for(auto& key : cmd_path) {
            // If key exists get sub tree
            try {
                auto& tmp = current_tree->at(key);

                std::visit( overloaded {
                        [&current_tree](Commands& val) {
                            current_tree = &val.command_tree;
                        },
                        [&key](Action& val) {
                            // TODO: Handle this to error/exception
                            fmt::print("Command {} already exists", key);
                        }
                    },
                    tmp
                );
            }
            // else create a new one
            catch(std::out_of_range) {
                // If its the last one create an Action
                if(!strcmp(key, *std::rbegin(cmd_path))) {
                    auto tmp = Action{
                        .num_args = arguments.size(),
                        .args = new Arg[arguments.size()],
                        .action = action_function,
                    };
                    current_tree->insert({key, tmp});
                    std::copy(arguments.begin(), arguments.end(), tmp.args);
                }
                // Else create a nested Commands map
                else {
                    auto inserted = current_tree->insert({key, Commands()});
                    // Update 'current_tree'
                    std::visit( overloaded {
                            [&current_tree](Commands& val) {
                                current_tree = &val.command_tree;
                            },
                            [](Action& val) {}
                        },
                        (*inserted.first).second
                    );
                }
            }
        }
    }

    void Commands::print(int identation) {
        if(identation == 0) {
            fmt::print("{{\n");
        }
        if(command_tree.size() == 0) {
            for(int i = 0; i < identation*2 ; ++i)
                fmt::print(" ");
            fmt::print("<empty map>\n");
        }
        for(auto& [key, val] : command_tree) {
            for(int i = 0; i < identation*2 ; ++i)
                fmt::print(" ");
            fmt::print("{}: {{\n", key);

            std::visit( overloaded {
                    [&identation](Commands& arg) {
                        arg.print(identation+1);
                    },
                    [&identation](Action& arg) {
                        for(int i = 0; i < (identation+1)*2 ; ++i)
                            fmt::print(" ");
                        fmt::print("<action(");
                        for(size_t i = 0 ; i < arg.num_args ; ++i) {
                            //fmt::print("{}{}", arg.args[i], i == (arg.num_args-1)  ? ", " : "");
                            switch (arg.args[i].type)
                            {
                            case Arg::INT:
                                fmt::print("Int{}",  i != (arg.num_args-1) ? ", " : "");
                                break;
                            case Arg::FLOAT:
                                fmt::print("Float{}",  i != (arg.num_args-1) ? ", " : "");
                                break;
                            case Arg::STRING:
                                fmt::print("String{}",  i != (arg.num_args-1) ? ", " : "");
                                break;
                            }
                        }
                        fmt::print(")>\n");
                    }
                },
                val
            );
        }
        for(int i = 0; i < identation*2 ; ++i)
            fmt::print(" ");
        fmt::print("}}\n");

        if(identation == 0)
            fmt::print("size = {}\n", command_tree.size());
    }


} // namespace kzn::console