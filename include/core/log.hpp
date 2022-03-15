#ifndef LOG_H
#define LOG_H

#include "config.hpp"
#include "core/utils.hpp"

#include <string>
#include <fmt/core.h>

#define BOLD        "\033[1m"
#define RESET       "\033[0m"
#define WHITE       "\033[1m\033[38;5;7m"
#define YELLOW      "\033[1m\033[33;1m"
#define BLUE        "\033[1m\033[38;5;36m"
#define RED         "\033[1m\033[0;31m"

namespace kzn
{
    class Log
    {
    public:

        static Log& get()
        {
            return instance;
        }

        // Simple string versions
        // Ex: Log::info("Hello World!") will write
        // [INFO] Hello World!
        static void info(std::string_view text);
        static void debug(std::string_view text);
        static void warning(std::string_view text);
        static void error(std::string_view text);

        // Format versions
        // Ex: Log::info("Hello {}!", "World") will write
        // [INFO] Hello World!
        template<typename... Args>
        static void info(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void debug(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void warning(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void error(fmt::format_string<Args...> in, Args&& ...args);

        // Context format versions
        // Ex: Log::info<"TEST">("Hello {}!", "World") will write
        // [INFO:TEST] Hello World!
        template<kzn::StringLiteral Str, typename... Args>
        static void info(fmt::format_string<Args...> in, Args&& ...args);
        template<kzn::StringLiteral Str, typename... Args>
        static void debug(fmt::format_string<Args...> in, Args&& ...args);
        template<kzn::StringLiteral Str, typename... Args>
        static void warning(fmt::format_string<Args...> in, Args&& ...args);
        template<kzn::StringLiteral Str, typename... Args>
        static void error(fmt::format_string<Args...> in, Args&& ...args);

    private:
        Log() = default;
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

    private:
        static Log instance;
    };

    template<typename... Args>
    void Log::info(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}INFO{}] ", WHITE, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<typename... Args>
    void Log::debug(fmt::format_string<Args...> in, Args&& ...args)
    {
        #ifdef KZN_DEBUG_LOGS
        fmt::print("[{}DEBUG{}] ", YELLOW, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<typename... Args>
    void Log::warning(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}WARNING{}] ", BLUE, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<typename... Args>
    void Log::error(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}ERROR{}] ", RED, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<kzn::StringLiteral Str, typename... Args>
    void Log::info(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}INFO{}:{}{}{}] ", WHITE, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<kzn::StringLiteral Str, typename... Args>
    void Log::debug(fmt::format_string<Args...> in, Args&& ...args)
    {
        #ifdef KZN_DEBUG_LOGS
        fmt::print("[{}DEBUG{}:{}{}{}] ", YELLOW, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<kzn::StringLiteral Str, typename... Args>
    void Log::warning(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}WARNING{}:{}{}{}] ", BLUE, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<kzn::StringLiteral Str, typename... Args>
    void Log::error(fmt::format_string<Args...> in, Args&& ...args)
    {
        fmt::print("[{}ERROR{}:{}{}{}] ", RED, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
    }
}

#endif // LOG_H
