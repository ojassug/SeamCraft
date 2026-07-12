#ifndef SEAMCRAFT_APPLICATION_HPP
#define SEAMCRAFT_APPLICATION_HPP

#include "Window.hpp"

// Owns the high-level application lifecycle and main loop.
class Application
{
public:
    Application();

    void run();

private:
    Window window;
};

#endif
