#include "logger.hpp"

#include <iostream>

namespace kzn
{
    void Logger::info(const std::string& text)
    {
        std::cout << "[\033[1m\033[38;5;7mINFO\033[0m] " << text << "\n";
    }

    void Logger::debug(const std::string& text)
    {
        #ifdef DEBUG
        std::cout << "[\033[1m\033[33;1mDEBUG\033[0m] " << text << "\n";
        #endif

    }

    void Logger::warning(const std::string& text)
    {
        std::cout << "[\033[1m\033[38;5;36mCOMMAND\033[0m] " << text << "\n";
    }

    void Logger::error(const std::string& text)
    {
        std::cout << "[\033[1m\033[0;31mERROR\033[0m] " << text << "\n";
    }

}