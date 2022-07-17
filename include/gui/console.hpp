#ifndef KZN_GUI_CONSOLE_HPP
#define KZN_GUI_CONSOLE_HPP

#include <fmt/core.h>

#include <unordered_map>
#include <variant>

namespace kzn::console {
    class Arg {
        public:
        enum Type: uint8_t {
            INT,
            FLOAT,
            STRING 
        };

        const char* name;
        enum Type type;
    };
    
    struct Action {
        size_t num_args;
        Arg*   args;
        void*  action;
    };
    
    class Commands {
        public:
        using CmdTree = std::unordered_map<std::string, std::variant<Commands, Action>>;
        private:
        CmdTree command_tree;

        public:
        Commands() = default;
        ~Commands() = default;
        
        void add_command(
            std::initializer_list<const char*> cmd_path,
            std::initializer_list<Arg> arguments,
            void* action_function);
        void print(int identation = 0);
    };
} // namespace kzn::console

#endif // KZN_GUI_CONSOLE_HPP