#include "Window.hpp"

#include <cstdint>
#include <optional>

namespace
{
constexpr unsigned int FrameRateLimit = 60;
constexpr std::uint8_t BackgroundRed = 32;
constexpr std::uint8_t BackgroundGreen = 36;
constexpr std::uint8_t BackgroundBlue = 48;
}

Window::Window(unsigned int width, unsigned int height, const std::string& title)
    : renderWindow(sf::VideoMode({width, height}), title),
      backgroundColor(BackgroundRed, BackgroundGreen, BackgroundBlue)
{
    renderWindow.setFramerateLimit(FrameRateLimit);
}

bool Window::isOpen() const
{
    return renderWindow.isOpen();
}

void Window::close()
{
    renderWindow.close();
}

void Window::handleEvents()
{
    // Poll every pending event so the window remains responsive.
    while (const std::optional event = renderWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            renderWindow.close();
        }
    }
}

void Window::beginFrame()
{
    renderWindow.clear(backgroundColor);
}

void Window::endFrame()
{
    renderWindow.display();
}

void Window::draw(const sf::Drawable& drawable)
{
    renderWindow.draw(drawable);
}

sf::RenderWindow& Window::getRenderWindow()
{
    return renderWindow;
}

const sf::RenderWindow& Window::getRenderWindow() const
{
    return renderWindow;
}
