#ifndef SEAMCRAFT_WINDOW_HPP
#define SEAMCRAFT_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

// Wraps SFML's RenderWindow so window setup, events, and drawing stay in one place.
class Window
{
public:
    Window(unsigned int width, unsigned int height, const std::string& title);

    bool isOpen() const;
    void handleEvents(const std::function<void(const sf::Event&)>& eventHandler = nullptr);
    void close();
    void beginFrame();
    void endFrame();
    void draw(const sf::Drawable& drawable);

    sf::RenderWindow& getRenderWindow();
    const sf::RenderWindow& getRenderWindow() const;

private:
    sf::RenderWindow renderWindow;
    sf::Color backgroundColor;
};

#endif
