#include "Application.hpp"

namespace
{
constexpr unsigned int WindowWidth = 1200;
constexpr unsigned int WindowHeight = 800;
constexpr const char WindowTitle[] = "SeamCraft";
}

Application::Application()
    : window(WindowWidth, WindowHeight, WindowTitle)
{
}

void Application::run()
{
    // Main loop: process input, prepare the frame, draw, then present it.
    while (window.isOpen())
    {
        window.handleEvents();

        window.beginFrame();
        window.endFrame();
    }
}
