#include "ImageManager.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <system_error>

namespace
{
constexpr float WindowPadding = 40.0f;
}

bool ImageManager::loadImage(const std::string& filePath)
{
    lastError.clear();

    if (filePath.empty())
    {
        lastError = "No image file path was provided.";
        return false;
    }

    const std::filesystem::path imagePath(filePath);
    std::error_code fileError;

    if (!std::filesystem::exists(imagePath, fileError))
    {
        lastError = "Image file was not found: " + filePath;
        return false;
    }

    if (!std::filesystem::is_regular_file(imagePath, fileError))
    {
        lastError = "Image path is not a regular file: " + filePath;
        return false;
    }

    if (!isSupportedFormat(filePath))
    {
        lastError = "Unsupported image format. Use PNG, JPG, or JPEG.";
        return false;
    }

    sf::Image loadedImage;
    if (!loadedImage.loadFromFile(filePath))
    {
        lastError = "Unable to load image. The file may be invalid or corrupted.";
        return false;
    }

    originalImage = loadedImage;
    currentImage = loadedImage;
    filename = extractFilename(filePath);

    if (!updateTexture())
    {
        return false;
    }

    return true;
}

bool ImageManager::resetImage()
{
    if (!hasImage())
    {
        lastError = "No image is loaded.";
        return false;
    }

    currentImage = originalImage;
    return updateTexture();
}

bool ImageManager::hasImage() const
{
    return sprite.has_value();
}

void ImageManager::fitToWindow(const sf::Vector2u& windowSize)
{
    if (!hasImage())
    {
        return;
    }

    const sf::Vector2u imageSize = currentImage.getSize();
    if (imageSize.x == 0 || imageSize.y == 0)
    {
        return;
    }

    const float availableWidth = std::max(1.0f, static_cast<float>(windowSize.x) - (WindowPadding * 2.0f));
    const float availableHeight = std::max(1.0f, static_cast<float>(windowSize.y) - (WindowPadding * 2.0f));

    const float widthScale = availableWidth / static_cast<float>(imageSize.x);
    const float heightScale = availableHeight / static_cast<float>(imageSize.y);
    scaleFactor = std::min({1.0f, widthScale, heightScale});

    const float displayedWidth = static_cast<float>(imageSize.x) * scaleFactor;
    const float displayedHeight = static_cast<float>(imageSize.y) * scaleFactor;
    const float xPosition = (static_cast<float>(windowSize.x) - displayedWidth) / 2.0f;
    const float yPosition = (static_cast<float>(windowSize.y) - displayedHeight) / 2.0f;

    sprite->setScale({scaleFactor, scaleFactor});
    sprite->setPosition({xPosition, yPosition});
}

void ImageManager::draw(sf::RenderTarget& renderTarget) const
{
    if (hasImage())
    {
        renderTarget.draw(*sprite);
    }
}

const std::string& ImageManager::getFilename() const
{
    return filename;
}

const sf::Image& ImageManager::getCurrentImage() const
{
    return currentImage;
}

unsigned int ImageManager::getWidth() const
{
    return currentImage.getSize().x;
}

unsigned int ImageManager::getHeight() const
{
    return currentImage.getSize().y;
}

float ImageManager::getScaleFactor() const
{
    return scaleFactor;
}

const std::string& ImageManager::getLastError() const
{
    return lastError;
}

bool ImageManager::isSupportedFormat(const std::string& filePath) const
{
    std::string extension = std::filesystem::path(filePath).extension().string();

    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });

    return extension == ".png" || extension == ".jpg" || extension == ".jpeg";
}

bool ImageManager::updateTexture()
{
    if (!texture.loadFromImage(currentImage))
    {
        sprite.reset();
        lastError = "Unable to create texture from image.";
        return false;
    }

    sprite.emplace(texture);
    scaleFactor = 1.0f;
    return true;
}

std::string ImageManager::extractFilename(const std::string& filePath) const
{
    return std::filesystem::path(filePath).filename().string();
}
