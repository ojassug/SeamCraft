#ifndef SEAMCRAFT_ENERGY_RENDERER_HPP
#define SEAMCRAFT_ENERGY_RENDERER_HPP

#include "EnergyCalculator.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <optional>

// Converts an energy map into a grayscale texture and draws it.
class EnergyRenderer
{
public:
    bool updateFromEnergyMap(const EnergyCalculator::EnergyMap& energyMap);

    bool hasVisualization() const;
    void fitToWindow(const sf::Vector2u& windowSize);
    void draw(sf::RenderTarget& renderTarget) const;

    unsigned int getWidth() const;
    unsigned int getHeight() const;

private:
    sf::Image createGrayscaleImage(const EnergyCalculator::EnergyMap& energyMap) const;
    float findMinimumEnergy(const EnergyCalculator::EnergyMap& energyMap) const;
    float findMaximumEnergy(const EnergyCalculator::EnergyMap& energyMap) const;
    std::uint8_t normalizeEnergy(float energy, float minimumEnergy, float maximumEnergy) const;

    sf::Image grayscaleImage;
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;
    unsigned int width = 0;
    unsigned int height = 0;
    float scaleFactor = 1.0f;
};

#endif
