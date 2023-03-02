#pragma once

#include "config.hpp"
#include "core/utils.hpp"

#include <string>
#include <fmt/core.h>

// TODO: Rename this macros
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define WHITE  "\033[1m\033[38;5;7m"
#define GRAY   "\033[1m\033[38;5;8m"
#define YELLOW "\033[1m\033[33;1m"
#define BLUE   "\033[1m\033[38;5;36m"
#define RED    "\033[1m\033[0;31m"

#ifndef KZN_LOG_LEVEL
#   define KZN_LOG_LEVEL 5
#endif

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
        static inline void error(std::string_view text);
        static inline void warning(std::string_view text);
        static inline void info(std::string_view text);
        static inline void debug(std::string_view text);
        static inline void trace(std::string_view text);

        // Format versions
        // Ex: Log::info("Hello {}!", "World") will write
        // [INFO] Hello World!
        template<typename... Args>
        static void error(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void warning(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void info(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void debug(fmt::format_string<Args...> in, Args&& ...args);
        template<typename... Args>
        static void trace(fmt::format_string<Args...> in, Args&& ...args);

        // Context format versions
        // Ex: Log::info<"TEST">("Hello {}!", "World") will write
        // [INFO:TEST] Hello World!
        template<StringLiteral Str, typename... Args>
        static void error(fmt::format_string<Args...> in, Args&& ...args);
        template<StringLiteral Str, typename... Args>
        static void warning(fmt::format_string<Args...> in, Args&& ...args);
        template<StringLiteral Str, typename... Args>
        static void info(fmt::format_string<Args...> in, Args&& ...args);
        template<StringLiteral Str, typename... Args>
        static void debug(fmt::format_string<Args...> in, Args&& ...args);
        template<StringLiteral Str, typename... Args>
        static void trace(fmt::format_string<Args...> in, Args&& ...args);

        private:
        Log() = default;
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        private:
        static Log instance;
    };


    void Log::error(std::string_view text)
    {
        #if KZN_LOG_LEVEL >= 1
        fmt::print("[{}ERROR{}] {}\n", RED, RESET, text);
        #endif
    }

    void Log::warning(std::string_view text)
    {
        #if KZN_LOG_LEVEL >= 2
        fmt::print("[{}WARNING{}] {}\n", BLUE, RESET, text);
        #endif
    }

    void Log::info(std::string_view text)
    {
        #if KZN_LOG_LEVEL >= 3
        fmt::print("[{}INFO{}] {}\n", WHITE, RESET, text);
        #endif
    }

    void Log::debug(std::string_view text)
    {
        #if KZN_LOG_LEVEL >= 4
        fmt::print("[{}DEBUG{}] {}\n", YELLOW, RESET, text);
        #endif
    }

    void Log::trace(std::string_view text)
    {
        #if KZN_LOG_LEVEL >= 5
        fmt::print("[{}TRACE{}] {}\n", GRAY, RESET, text);
        #endif
    }


    template<typename... Args>
    void Log::error(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 1
        fmt::print("[{}ERROR{}] ", RED, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<typename... Args>
    void Log::warning(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 2
        fmt::print("[{}WARNING{}] ", BLUE, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<typename... Args>
    void Log::info(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 3
        fmt::print("[{}INFO{}] ", WHITE, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<typename... Args>
    void Log::debug(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 4
        fmt::print("[{}DEBUG{}] ", YELLOW, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<typename... Args>
    void Log::trace(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 5
        fmt::print("[{}TRACE{}] ", GRAY, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }


    template<StringLiteral Str, typename... Args>
    void Log::error(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 1
        fmt::print("[{}ERROR{}:{}{}{}] ", RED, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<StringLiteral Str, typename... Args>
    void Log::warning(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 2
        fmt::print("[{}WARNING{}:{}{}{}] ", BLUE, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<StringLiteral Str, typename... Args>
    void Log::info(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 3
        fmt::print("[{}INFO{}:{}{}{}] ", WHITE, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<StringLiteral Str, typename... Args>
    void Log::debug(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 4
        fmt::print("[{}DEBUG{}:{}{}{}] ", YELLOW, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }

    template<StringLiteral Str, typename... Args>
    void Log::trace(fmt::format_string<Args...> in, Args&& ...args)
    {
        #if KZN_LOG_LEVEL <= 5
        fmt::print("[{}TRACE{}:{}{}{}] ", GRAY, RESET, BOLD, Str.value, RESET);
        fmt::print(in, std::forward<Args>(args)...);
        fmt::print("\n");
        #endif
    }
}