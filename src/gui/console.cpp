#include "gui/console.hpp"

#include <fmt/core.h>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <charconv>

#include "utils/string.hpp"
#include "utils/variant.hpp"

namespace kzn::console {
    CommandError Commands::add_command(
        std::initializer_list<const char*> cmd_path,
        std::initializer_list<Arg> arguments,
        std::function<void(Arg::Any*)>&& action_function)
    {
        CmdTree* current_tree = &command_tree;
        for(auto& key : cmd_path) {
            // If key exists get sub tree
            try {
                auto& tmp = current_tree->at(key);

                CommandError error = NONE;
                std::visit( overloaded {
                        [&current_tree](Commands& val) {
                            current_tree = &val.command_tree;
                        },
                        [&key,&error](Action&) {
                            error = CMD_EXISTS;
                        }
                    },
                    tmp
                );
                if(error != NONE)
                    return error;
            }
            // else create a new one
            catch(std::out_of_range) {
                // If its the last one create an Action
                if(!strcmp(key, *std::rbegin(cmd_path))) {
                    auto tmp = Action{
                        .num_args = arguments.size(),
                        .args = new Arg[arguments.size()],
                        .action = std::move(action_function),
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
                            [](Action&) {}
                        },
                        (*inserted.first).second
                    );
                }
            }
        }
        return NONE;
    }

    void Commands::print(int identation) const {
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
                    [&identation](const Commands& arg) {
                        arg.print(identation+1);
                    },
                    [&identation](const Action& arg) {
                        for(int i = 0; i < (identation+1)*2 ; ++i)
                            fmt::print(" ");
                        fmt::print("<action(");
                        for(size_t i = 0 ; i < arg.num_args ; ++i) {
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
    }

    CommandError Commands::execute(const char* cmd) const {
        auto splited_str = kzn::split(cmd);
        const CmdTree* current_tree = &command_tree;
        for(auto it = splited_str.begin(); it < splited_str.end(); ++it) {
            auto str = *it;
            // Get value
            try {
                auto& value = current_tree->at(std::string(str));
                bool break_loop = false;
                CommandError error = NONE;
                std::visit( overloaded {
                        [&current_tree](const Commands& val) {
                            current_tree = &val.command_tree;
                        },
                        [&break_loop, &it, &splited_str, &error](const Action& action) {
                            // Get number of arguments provided
                            size_t num_of_args = 0;
                            for(auto tmp_it = it+1; tmp_it < splited_str.end(); ++tmp_it)
                                ++num_of_args;
                            
                            if(num_of_args != action.num_args) {
                                error = INVALID_NUM_ARGS;
                            }
                            else {
                                // TODO: Execute command
                                Arg::Any* pass_args = new Arg::Any[num_of_args];
                                it += 1;
                                for(size_t i = 0; (it+i) < splited_str.end(); ++i) {
                                    const auto arg = *(it+i);
                                    switch (action.args[i].type)
                                    {
                                    case Arg::INT: {
                                        // Convert str_view to int
                                        int parsed;
                                        auto res = std::from_chars(arg.data(), arg.data()+arg.size(), parsed);
                                        if(res.ec == std::errc::invalid_argument) 
                                            error = ARG_CONVERSION_ERROR;
                                        else 
                                            pass_args[i] = parsed;
                                        break;
                                    }
                                    case Arg::FLOAT: {
                                        // Convert str_view to float
                                        float parsed;
                                        auto res = std::from_chars(arg.data(), arg.data()+arg.size(), parsed);
                                        if(res.ec == std::errc::invalid_argument) 
                                            error = ARG_CONVERSION_ERROR;
                                        else 
                                            pass_args[i] = parsed;
                                        break;
                                    }
                                    case Arg::STRING: {
                                        // Copy str_view to char*
                                        char* parsed = new char[arg.size()];
                                        std::copy(arg.data(), arg.data()+arg.size(), parsed);
                                        pass_args[i] = parsed;
                                        break;
                                    }
                                    }
                                }
                                if(!error)
                                    action.action(pass_args);
                            }
                            break_loop = true;
                        }
                    },
                    value
                );
                if(error != NONE)
                    return error;
                if(break_loop)
                    break;
            }
            // Command doesn't exist
            catch(std::out_of_range) {
                return CMD_DOESNT_EXISTS;
                break;
            }
            // Action call has bad argument variant access
            catch(std::bad_variant_access) {
                return INVALID_ARG_ACCESS;
                break;
            }
        }
        return NONE;
    }
} // namespace kzn::console