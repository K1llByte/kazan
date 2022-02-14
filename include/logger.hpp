#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#define DEBUG

namespace kzn
{
    class Logger
    {
    public:

        static Logger& get()
        {
            static Logger instance;
            return instance;
        }

        static void info(const std::string&);
        static void debug(const std::string&);
        static void warning(const std::string&);
        static void error(const std::string&);

    private:
        Logger() {}
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
    };
}

#endif // LOGGER_H
