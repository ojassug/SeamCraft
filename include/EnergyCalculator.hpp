#ifndef SEAMCRAFT_ENERGY_CALCULATOR_HPP
#define SEAMCRAFT_ENERGY_CALCULATOR_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Computes and stores the per-pixel energy values used by seam carving.
class EnergyCalculator
{
public:
    using EnergyMap = std::vector<std::vector<float>>;

    void setImage(const sf::Image& image);
    void calculate();

    const EnergyMap& getEnergyMap() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

private:
    float getGray(unsigned int x, unsigned int y) const;
    unsigned int clampX(int x) const;
    unsigned int clampY(int y) const;

    sf::Image sourceImage;
    EnergyMap energyMap;
    unsigned int width = 0;
    unsigned int height = 0;
};

#endif
