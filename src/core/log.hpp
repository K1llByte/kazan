#pragma once

#include <fmt/core.h>
#include <string_view>

// TODO: Improve in the future
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define WHITE "\033[1m\033[38;5;7m"
#define GRAY "\033[1m\033[38;5;8m"
#define YELLOW "\033[1m\033[33;1m"
#define BLUE "\033[1m\033[38;5;36m"
#define RED "\033[1m\033[0;31m"

namespace kzn {

class Log {
public:
    // Simple string versions
    // Ex: Log::info("Hello World!") will write
    // [INFO] Hello World!
    static void error(std::string_view text);
    static void warning(std::string_view text);
    static void info(std::string_view text);
    static void debug(std::string_view text);
    static void trace(std::string_view text);

    // Format versions
    // Ex: Log::info("Hello {}!", "World") will write
    // [INFO] Hello World!
    template<typename... Args>
    static void error(fmt::format_string<Args...> in, Args&&... args);
    template<typename... Args>
    static void warning(fmt::format_string<Args...> in, Args&&... args);
    template<typename... Args>
    static void info(fmt::format_string<Args...> in, Args&&... args);
    template<typename... Args>
    static void debug(fmt::format_string<Args...> in, Args&&... args);
    template<typename... Args>
    static void trace(fmt::format_string<Args...> in, Args&&... args);
};

template<typename... Args>
void Log::error(fmt::format_string<Args...> in, Args&&... args) {
    fmt::print("[{}ERROR{}] ", RED, RESET);
    fmt::print(in, std::forward<Args>(args)...);
    fmt::print("\n");
}

template<typename... Args>
void Log::warning(fmt::format_string<Args...> in, Args&&... args) {
    fmt::print("[{}WARNING{}] ", YELLOW, RESET);
    fmt::print(in, std::forward<Args>(args)...);
    fmt::print("\n");
}

template<typename... Args>
void Log::info(fmt::format_string<Args...> in, Args&&... args) {
    fmt::print("[{}INFO{}] ", WHITE, RESET);
    fmt::print(in, std::forward<Args>(args)...);
    fmt::print("\n");
}

template<typename... Args>
void Log::debug(fmt::format_string<Args...> in, Args&&... args) {
    fmt::print("[{}DEBUG{}] ", BLUE, RESET);
    fmt::print(in, std::forward<Args>(args)...);
    fmt::print("\n");
}

template<typename... Args>
void Log::trace(fmt::format_string<Args...> in, Args&&... args) {
    fmt::print("[{}TRACE{}] ", GRAY, RESET);
    fmt::print(in, std::forward<Args>(args)...);
    fmt::print("\n");
}

} // namespace kzn