#include "core/log.hpp"

namespace kzn
{
    void Log::info(std::string_view text)
    {
        fmt::print("[{}INFO{}] {}\n", WHITE, RESET, text);
    }

    void Log::debug(std::string_view text)
    {
        #ifdef KZN_DEBUG_LOGS
        fmt::print("[{}DEBUG{}] {}\n", YELLOW, RESET, text);
        #endif
    }

    void Log::warning(std::string_view text)
    {
        fmt::print("[{}WARNING{}] {}\n", BLUE, RESET, text);
    }

    void Log::error(std::string_view text)
    {
        fmt::print("[{}ERROR{}] {}\n", RED, RESET, text);
    }
}