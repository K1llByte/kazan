#ifndef KZN_GUI_CONSOLE_HPP
#define KZN_GUI_CONSOLE_HPP

#include <fmt/core.h>

#include <unordered_map>
#include <functional>
#include <variant>

namespace kzn::console {
    struct Arg {
        using Any = std::variant<int,float,char*>;
        enum Type: uint8_t {
            INT,
            FLOAT,
            STRING
        };

        const char* name;
        enum Type type;
    };

    inline Arg Int(const char* _name) { return Arg{_name, Arg::INT}; }
    inline Arg Float(const char* _name) { return Arg{_name, Arg::FLOAT}; }
    inline Arg String(const char* _name) { return Arg{_name, Arg::STRING}; }
    
    struct Action {
        size_t                         num_args;
        Arg*                           args;
        std::function<void(Arg::Any*)> action;
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
            std::function<void(Arg::Any*)>&& action_function);
        inline void add_command(
            std::initializer_list<const char*> cmd_path,
            std::function<void(Arg::Any*)>&& action_function);
        void print(int identation = 0) const;
        void execute(const char* cmd) const;
    };

    //void testing(const char* asd);
} // namespace kzn::console

namespace kzn::console {
    inline void Commands::add_command(
        std::initializer_list<const char*> cmd_path,
        std::function<void(Arg::Any*)>&& action_function)
    {
        add_command(cmd_path, {}, std::move(action_function));
    }
}

#endif // KZN_GUI_CONSOLE_HPP