#ifndef SEAMCRAFT_IMAGE_MANAGER_HPP
#define SEAMCRAFT_IMAGE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

// Owns image state needed now for display and later for seam carving.
class ImageManager
{
public:
    bool loadImage(const std::string& filePath);
    bool resetImage();
    bool setCurrentImage(const sf::Image& image);
    bool saveCurrentImage(const std::string& filePath);

    bool hasImage() const;
    void fitToWindow(const sf::Vector2u& windowSize);
    void draw(sf::RenderTarget& renderTarget) const;

    const std::string& getFilename() const;
    const sf::Image& getCurrentImage() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    float getScaleFactor() const;
    const std::string& getLastError() const;

private:
    bool isSupportedFormat(const std::string& filePath) const;
    bool updateTexture();
    std::string extractFilename(const std::string& filePath) const;

    sf::Image originalImage;
    sf::Image currentImage;
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;

    std::string filename;
    std::string lastError;
    float scaleFactor = 1.0f;
};

#endif
