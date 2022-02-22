#include "core/log.hpp"

namespace kzn
{
    void Log::info(const std::string& text)
    {
        fmt::print("[{}INFO{}] {}\n", WHITE, RESET, text);
    }

    void Log::debug(const std::string& text)
    {
        #ifdef KZN_DEBUG_LOGS
        fmt::print("[{}DEBUG{}] {}\n", YELLOW, RESET, text);
        #endif
    }

    void Log::warning(const std::string& text)
    {
        fmt::print("[{}WARNING{}] {}\n", BLUE, RESET, text);
    }

    void Log::error(const std::string& text)
    {
        fmt::print("[{}ERROR{}] {}\n", RED, RESET, text);
    }
}