#pragma once

#include "vk/error.hpp"

namespace kzn {

//! Pure interface for implementing an app
struct App {
    virtual void run() = 0;
};

} // namespace kzn

#define KZN_CREATE_APP(app_type)                                               \
    int main() {                                                               \
        try {                                                                  \
            app_type app;                                                      \
            app.run();                                                         \
        }                                                                      \
        catch (const kzn::vk::ResultError& re) {                               \
            kzn::Log::error("Fatal Error: {}", re.message());                  \
        }                                                                      \
        catch (const kzn::LoadingError& le) {                                  \
            kzn::Log::error("Loading Error: {}", le.message);                  \
        }                                                                      \
        return EXIT_SUCCESS;                                                   \
    }