#pragma once

namespace kzn {

//! Pure interface for implementing an app
struct App {
    virtual void run() = 0;
};

} // namespace kzn

#define KZN_CREATE_APP(class_name) \
    int main() {                   \
        class_name app;            \
        app.run();                 \
        return EXIT_SUCCESS;       \
    }