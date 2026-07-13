#ifndef SEAMCRAFT_APPLICATION_HPP
#define SEAMCRAFT_APPLICATION_HPP

#include "ImageManager.hpp"
#include "Window.hpp"

// Owns the high-level application lifecycle and main loop.
class Application
{
public:
    Application();

    void run();

private:
    void loadStartupImage();
    void handleEvent(const sf::Event& event);
    void openImage();
    void resetImage();
    void setLoadedStatus();
    void setStatus(const std::string& message);
    void updateWindowTitle();

    Window window;
    ImageManager imageManager;
    std::string statusMessage;
};

#endif
