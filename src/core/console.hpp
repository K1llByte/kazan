#pragma once

#include "core/console_types.hpp"
#include "core/log.hpp"
#include "core/string.hpp"
#include "core/string_hash.hpp"
#include "core/traits.hpp"

#include <charconv>
#include <functional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace kzn {

template<typename Tuple, std::size_t Index = 0>
void convert_to_args_tuple_aux(
    Tuple& args_tuple,
    std::span<std::string_view> cmd_args
) {
    if constexpr (Index < std::tuple_size<Tuple>::value) {
        using Arg = std::tuple_element_t<Index, Tuple>;
        std::get<Index>(args_tuple) =
            ConsoleTypeTraits<Arg>::convert_to(cmd_args[Index]);
        convert_to_args_tuple_aux<Tuple, Index + 1>(args_tuple, cmd_args);
    }
}

template<typename ArgsTuple>
ArgsTuple convert_to_args_tuple(std::span<std::string_view> cmd_args) {
    if (cmd_args.size() != std::tuple_size_v<ArgsTuple>) {
        throw std::runtime_error("Invalid argument count!");
    }
    ArgsTuple args_tuple{};
    convert_to_args_tuple_aux(args_tuple, cmd_args);
    return args_tuple;
}

class Console {
public:
    // Ctor
    Console() = default;
    // Copy
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
    // Move
    Console(Console&&) = default;
    Console& operator=(Console&&) = default;
    // Dtor
    ~Console() = default;

    //! Create command.
    //! \return true if command was created, false otherwise
    template<typename Fn>
    bool create_cmd(std::string cmd_name, Fn&& fn) {
        using ArgsTuple = typename FunctionTraits<Fn>::ArgTypes;

        auto execute_cmd_fn =
            [fn = std::move(fn)](std::span<std::string_view> cmd_args) {
                std::apply(fn, convert_to_args_tuple<ArgsTuple>(cmd_args));
            };

        auto [_, inserted] =
            m_commands.try_emplace(cmd_name, std::move(execute_cmd_fn));
        if (inserted) {
            // Insert cmd sorted
            m_commands_names.insert(
                std::upper_bound(
                    m_commands_names.begin(), m_commands_names.end(), cmd_name
                ),
                std::move(cmd_name)
            );
        }
        return inserted;
    }

    //! Delete command.
    //! \return true if command was deleted, false otherwise
    bool delete_cmd(StringHash cmd_name) {
        return m_commands.erase(cmd_name) != 0;
    }

    //! Execute command
    //! \return false if command does not exist.
    bool execute_cmd(std::string_view cmd) {
        // TODO: Implement a more sophisticated tokenizer
        auto splitted_cmd = split(cmd, std::array{' ', '\0'});
        if (splitted_cmd.empty()) {
            return false;
        }

        auto it = m_commands.find(splitted_cmd[0]);
        if (it != m_commands.end()) {
            try {
                std::invoke(
                    it->second,
                    std::span{splitted_cmd.data() + 1, splitted_cmd.size() - 1}
                );
            }
            catch (const std::runtime_error& re) {
                Log::error("Console error: {}", re.what());
                return false;
            }
            return true;
        }

        Log::error("Command not found");
        return false;
    }

    std::span<const std::string> cmds() const {
        return std::span{m_commands_names.data(), m_commands_names.size()};
    }

private:
    using CmdFn = std::function<void(std::span<std::string_view>)>;
    std::unordered_map<StringHash, CmdFn> m_commands;
    //! TODO: Have this vector sorted
    //! This container contains only unique strings
    std::vector<std::string> m_commands_names;
};

} // namespace kzn