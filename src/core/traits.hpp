#pragma once

// #define SINGLETON_USE_ASSERT

#define SINGLETON_THROWS false
#if defined(SINGLETON_USE_ASSERT)
#define SINGLETON_CHECK(expr, msg) assert(expr&& msg)
#elif defined(SINGLETON_USE_THROW)
#define SINGLETON_CHECK(expr, msg)                                             \
    if (!expr) {                                                               \
        throw std::runtime_error(msg)                                          \
    }
#define SINGLETON_THROWS true
#else
#define SINGLETON_CHECK(expr, msg) (void)nullptr
#endif

namespace kzn {

struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
    NoCopy& operator=(const NoCopy&) = delete;
};

struct NoMove {
    NoMove() = default;
    NoMove(NoMove&&) = delete;
    NoMove& operator=(NoMove&&) = delete;
};

struct NoCopyMove
    : public NoCopy
    , public NoMove {};

template<typename T>
class Singleton {
public:
    // Ctor
    Singleton() noexcept(SINGLETON_THROWS) {
        SINGLETON_CHECK(!exists(), "Instance can only be created once!");
        s_singleton = static_cast<T*>(this);
    }

    // No Copy
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // Move
    Singleton(Singleton&&) { s_singleton = static_cast<T*>(this); }
    Singleton& operator=(Singleton&&) { s_singleton = static_cast<T*>(this); }

    // Dtor
    ~Singleton() {
        if (this == s_singleton) {
            s_singleton = nullptr;
        }
    }

    [[nodiscard]]
    static bool exists() {
        return s_singleton != nullptr;
    }

    [[nodiscard]]
    static T& singleton() {
        SINGLETON_CHECK(exists(), "Instance does not exist!");
        return *s_singleton;
    }

private:
    static inline T* s_singleton = nullptr;
};
} // namespace kzn