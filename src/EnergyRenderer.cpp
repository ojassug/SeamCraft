#include "EnergyRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace
{
constexpr float WindowPadding = 40.0f;
}

bool EnergyRenderer::updateFromEnergyMap(const EnergyCalculator::EnergyMap& energyMap)
{
    if (energyMap.empty() || energyMap.front().empty())
    {
        width = 0;
        height = 0;
        sprite.reset();
        return false;
    }

    height = static_cast<unsigned int>(energyMap.size());
    width = static_cast<unsigned int>(energyMap.front().size());
    grayscaleImage = createGrayscaleImage(energyMap);

    if (!texture.loadFromImage(grayscaleImage))
    {
        sprite.reset();
        return false;
    }

    sprite.emplace(texture);
    scaleFactor = 1.0f;
    return true;
}

bool EnergyRenderer::hasVisualization() const
{
    return sprite.has_value();
}

void EnergyRenderer::fitToWindow(const sf::Vector2u& windowSize)
{
    if (!hasVisualization() || width == 0 || height == 0)
    {
        return;
    }

    const float availableWidth = std::max(1.0f, static_cast<float>(windowSize.x) - (WindowPadding * 2.0f));
    const float availableHeight = std::max(1.0f, static_cast<float>(windowSize.y) - (WindowPadding * 2.0f));

    const float widthScale = availableWidth / static_cast<float>(width);
    const float heightScale = availableHeight / static_cast<float>(height);
    scaleFactor = std::min({1.0f, widthScale, heightScale});

    const float displayedWidth = static_cast<float>(width) * scaleFactor;
    const float displayedHeight = static_cast<float>(height) * scaleFactor;
    const float xPosition = (static_cast<float>(windowSize.x) - displayedWidth) / 2.0f;
    const float yPosition = (static_cast<float>(windowSize.y) - displayedHeight) / 2.0f;

    sprite->setScale({scaleFactor, scaleFactor});
    sprite->setPosition({xPosition, yPosition});
}

void EnergyRenderer::draw(sf::RenderTarget& renderTarget) const
{
    if (hasVisualization())
    {
        renderTarget.draw(*sprite);
    }
}

unsigned int EnergyRenderer::getWidth() const
{
    return width;
}

unsigned int EnergyRenderer::getHeight() const
{
    return height;
}

sf::Image EnergyRenderer::createGrayscaleImage(const EnergyCalculator::EnergyMap& energyMap) const
{
    const float minimumEnergy = findMinimumEnergy(energyMap);
    const float maximumEnergy = findMaximumEnergy(energyMap);

    sf::Image image;
    image.resize({width, height}, sf::Color::Black);

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            const std::uint8_t grayValue = normalizeEnergy(energyMap[y][x], minimumEnergy, maximumEnergy);
            image.setPixel({x, y}, sf::Color(grayValue, grayValue, grayValue));
        }
    }

    return image;
}

float EnergyRenderer::findMinimumEnergy(const EnergyCalculator::EnergyMap& energyMap) const
{
    float minimumEnergy = energyMap.front().front();

    for (const std::vector<float>& row : energyMap)
    {
        for (float energy : row)
        {
            minimumEnergy = std::min(minimumEnergy, energy);
        }
    }

    return minimumEnergy;
}

float EnergyRenderer::findMaximumEnergy(const EnergyCalculator::EnergyMap& energyMap) const
{
    float maximumEnergy = energyMap.front().front();

    for (const std::vector<float>& row : energyMap)
    {
        for (float energy : row)
        {
            maximumEnergy = std::max(maximumEnergy, energy);
        }
    }

    return maximumEnergy;
}

std::uint8_t EnergyRenderer::normalizeEnergy(float energy, float minimumEnergy, float maximumEnergy) const
{
    // Normalize energy values so the lowest energy becomes black and the highest becomes white.
    const float energyRange = maximumEnergy - minimumEnergy;
    if (energyRange <= 0.0f)
    {
        return 0;
    }

    const float normalizedValue = (energy - minimumEnergy) / energyRange;
    const float grayscaleValue = std::clamp(normalizedValue * 255.0f, 0.0f, 255.0f);
    return static_cast<std::uint8_t>(std::round(grayscaleValue));
}
