#ifndef TESTAPP_H
#define TESTAPP_H

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "game_object.hpp"
// #include "ui.hpp"

namespace kzn
{

class TestApp
{
public:

    static constexpr int WIDTH = 1700;
    static constexpr int HEIGHT = 900;

private:

    Window   _window;
    Instance _instance;
    Device   _device;
    Renderer _renderer;
    // Interface _interface;
    // std::unique_ptr<Renderer> _renderer = nullptr;
    std::vector<GameObject>   _game_objects;


public:

    TestApp();
    ~TestApp();

    void run();

private:

    void load_game_objects();

};

}

#endif // TESTAPP_H
