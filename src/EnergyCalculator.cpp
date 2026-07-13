#include "EnergyCalculator.hpp"

#include <cmath>
#include <stdexcept>

void EnergyCalculator::setImage(const sf::Image& image)
{
    sourceImage = image;
    const sf::Vector2u imageSize = sourceImage.getSize();
    width = imageSize.x;
    height = imageSize.y;
    energyMap.clear();
}

void EnergyCalculator::calculate()
{
    energyMap.assign(height, std::vector<float>(width, 0.0f));

    if (width == 0 || height == 0)
    {
        return;
    }

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            const float topLeft = getGray(clampX(static_cast<int>(x) - 1), clampY(static_cast<int>(y) - 1));
            const float top = getGray(x, clampY(static_cast<int>(y) - 1));
            const float topRight = getGray(clampX(static_cast<int>(x) + 1), clampY(static_cast<int>(y) - 1));
            const float left = getGray(clampX(static_cast<int>(x) - 1), y);
            const float right = getGray(clampX(static_cast<int>(x) + 1), y);
            const float bottomLeft = getGray(clampX(static_cast<int>(x) - 1), clampY(static_cast<int>(y) + 1));
            const float bottom = getGray(x, clampY(static_cast<int>(y) + 1));
            const float bottomRight = getGray(clampX(static_cast<int>(x) + 1), clampY(static_cast<int>(y) + 1));

            // Sobel horizontal kernel:
            // -1  0  1
            // -2  0  2
            // -1  0  1
            const float horizontalGradient =
                (-1.0f * topLeft) + (1.0f * topRight) +
                (-2.0f * left) + (2.0f * right) +
                (-1.0f * bottomLeft) + (1.0f * bottomRight);

            // Sobel vertical kernel:
            // -1 -2 -1
            //  0  0  0
            //  1  2  1
            const float verticalGradient =
                (-1.0f * topLeft) + (-2.0f * top) + (-1.0f * topRight) +
                (1.0f * bottomLeft) + (2.0f * bottom) + (1.0f * bottomRight);

            energyMap[y][x] = std::sqrt((horizontalGradient * horizontalGradient) +
                                        (verticalGradient * verticalGradient));
        }
    }
}

float EnergyCalculator::getEnergy(unsigned int x, unsigned int y) const
{
    if (y >= energyMap.size() || x >= energyMap[y].size())
    {
        throw std::out_of_range("Energy coordinate is outside the energy map.");
    }

    return energyMap[y][x];
}

const EnergyCalculator::EnergyMap& EnergyCalculator::getEnergyMap() const
{
    return energyMap;
}

unsigned int EnergyCalculator::getWidth() const
{
    return width;
}

unsigned int EnergyCalculator::getHeight() const
{
    return height;
}

float EnergyCalculator::getGray(unsigned int x, unsigned int y) const
{
    const sf::Color color = sourceImage.getPixel({x, y});
    return (0.299f * static_cast<float>(color.r)) +
           (0.587f * static_cast<float>(color.g)) +
           (0.114f * static_cast<float>(color.b));
}

unsigned int EnergyCalculator::clampX(int x) const
{
    if (x < 0)
    {
        return 0;
    }

    if (x >= static_cast<int>(width))
    {
        return width - 1;
    }

    return static_cast<unsigned int>(x);
}

unsigned int EnergyCalculator::clampY(int y) const
{
    if (y < 0)
    {
        return 0;
    }

    if (y >= static_cast<int>(height))
    {
        return height - 1;
    }

    return static_cast<unsigned int>(y);
}
