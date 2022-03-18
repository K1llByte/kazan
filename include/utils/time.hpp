#ifndef KZN_UTILS_TIME_HPP
#define KZN_UTILS_TIME_HPP

#include <chrono>

namespace kzn
{
    struct Time
    {
        // Gives time in milliseconds
        static inline uint64_t now()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
        }
    };
} // namespace kzn

#endif // KZN_UTILS_TIME_HPP