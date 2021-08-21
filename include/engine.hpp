#ifndef ENGINE_H
#define ENGINE_H

#include "types.hpp"

namespace kzn
{

class Engine
{
public:

    bool _is_initialized{ false };
    int _frame_number{ 0 };

    VkExtent2D _window_extent{ 1700 , 900 };

    struct GLFWwindow* _window{ nullptr };

public:

    // Initializes necessary objects
    void init();

    // Destroys every objects
    void cleanup();

    // Draw loop
    void draw();

    // Run main loop
    void run();
};

}

#endif // ENGINE_H