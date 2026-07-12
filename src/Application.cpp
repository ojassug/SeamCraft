#include "Application.hpp"

#include <iostream>

namespace
{
constexpr unsigned int WindowWidth = 1200;
constexpr unsigned int WindowHeight = 800;
constexpr const char WindowTitle[] = "SeamCraft";
constexpr const char StartupImagePath[] = "assets/images/sample.png";
}

Application::Application()
    : window(WindowWidth, WindowHeight, WindowTitle)
{
    loadStartupImage();
}

void Application::run()
{
    // Main loop: process input, prepare the frame, draw, then present it.
    while (window.isOpen())
    {
        window.handleEvents();
        imageManager.fitToWindow(window.getRenderWindow().getSize());

        window.beginFrame();
        imageManager.draw(window.getRenderWindow());
        window.endFrame();
    }
}

void Application::loadStartupImage()
{
    if (imageManager.loadImage(StartupImagePath))
    {
        std::cout << "Loaded startup image: " << imageManager.getFilename() << '\n';
    }
    else
    {
        std::cout << "Startup image was not loaded. " << imageManager.getLastError() << '\n';
        std::cout << "The application will continue without an image.\n";
    }
}
