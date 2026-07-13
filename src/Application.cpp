#include "Application.hpp"

#include "tinyfiledialogs.h"

#include <iostream>
#include <sstream>

namespace
{
constexpr unsigned int WindowWidth = 1200;
constexpr unsigned int WindowHeight = 800;
constexpr const char WindowTitle[] = "SeamCraft";
constexpr const char StartupImagePath[] = "assets/images/sample.png";
constexpr const char TitleHelpText[] = " | O: Open Image | R: Reset";
}

Application::Application()
    : window(WindowWidth, WindowHeight, WindowTitle)
{
    loadStartupImage();
    updateWindowTitle();
}

void Application::run()
{
    // Main loop: process input, prepare the frame, draw, then present it.
    while (window.isOpen())
    {
        window.handleEvents([this](const sf::Event& event) {
            handleEvent(event);
        });
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
        setLoadedStatus();
    }
    else
    {
        std::cout << "Startup image was not loaded. " << imageManager.getLastError() << '\n';
        std::cout << "The application will continue without an image.\n";
        setStatus("Startup image was not loaded. " + imageManager.getLastError());
    }
}

void Application::handleEvent(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr)
    {
        return;
    }

    if (keyPressed->code == sf::Keyboard::Key::O)
    {
        openImage();
    }
    else if (keyPressed->code == sf::Keyboard::Key::R)
    {
        resetImage();
    }
}

void Application::openImage()
{
    const char* filterPatterns[] = {"*.png", "*.jpg", "*.jpeg"};
    const char* selectedPath = tinyfd_openFileDialog(
        "Open Image",
        "",
        3,
        filterPatterns,
        "Image files (*.png, *.jpg, *.jpeg)",
        0);

    if (selectedPath == nullptr)
    {
        setStatus("Open image canceled.");
        return;
    }

    if (imageManager.loadImage(selectedPath))
    {
        setLoadedStatus();
    }
    else
    {
        setStatus("Failed to load image. " + imageManager.getLastError());
    }
}

void Application::resetImage()
{
    if (imageManager.resetImage())
    {
        setStatus("Reset image: " + imageManager.getFilename());
    }
    else
    {
        setStatus("Reset failed. " + imageManager.getLastError());
    }
}

void Application::setLoadedStatus()
{
    std::ostringstream status;
    status << "Loaded: " << imageManager.getFilename()
           << " | " << imageManager.getWidth()
           << " x " << imageManager.getHeight();

    setStatus(status.str());
}

void Application::setStatus(const std::string& message)
{
    statusMessage = message;
    std::cout << statusMessage << '\n';
    updateWindowTitle();
}

void Application::updateWindowTitle()
{
    std::string title = WindowTitle;
    if (!statusMessage.empty())
    {
        title += " | " + statusMessage;
    }

    title += TitleHelpText;
    window.getRenderWindow().setTitle(title);
}
