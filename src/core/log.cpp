#include "core/log.hpp"

namespace kzn
{
    void Log::error(std::string_view text)
    {
        fmt::print("[{}ERROR{}] {}\n", RED, RESET, text);
    }

    void Log::warning(std::string_view text)
    {
        fmt::print("[{}WARNING{}] {}\n", BLUE, RESET, text);
    }

    void Log::info(std::string_view text)
    {
        fmt::print("[{}INFO{}] {}\n", WHITE, RESET, text);
    }

    void Log::debug(std::string_view text)
    {
        fmt::print("[{}DEBUG{}] {}\n", YELLOW, RESET, text);
    }

    void Log::trace(std::string_view text)
    {
        fmt::print("[{}TRACE{}] {}\n", GRAY, RESET, text);
    }
}