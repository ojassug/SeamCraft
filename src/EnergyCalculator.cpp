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
    energyMap.resize(height);
    for (std::vector<float>& row : energyMap)
    {
        row.assign(width, 0.0f);
    }

    if (width == 0 || height == 0)
    {
        return;
    }

    std::vector<float> grayscaleValues(width * height, 0.0f);
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            grayscaleValues[(y * width) + x] = getGray(x, y);
        }
    }

    for (unsigned int y = 0; y < height; ++y)
    {
        const unsigned int topY = clampY(static_cast<int>(y) - 1);
        const unsigned int bottomY = clampY(static_cast<int>(y) + 1);
        std::vector<float>& energyRow = energyMap[y];

        for (unsigned int x = 0; x < width; ++x)
        {
            const unsigned int leftX = clampX(static_cast<int>(x) - 1);
            const unsigned int rightX = clampX(static_cast<int>(x) + 1);

            const float topLeft = grayscaleValues[(topY * width) + leftX];
            const float top = grayscaleValues[(topY * width) + x];
            const float topRight = grayscaleValues[(topY * width) + rightX];
            const float left = grayscaleValues[(y * width) + leftX];
            const float right = grayscaleValues[(y * width) + rightX];
            const float bottomLeft = grayscaleValues[(bottomY * width) + leftX];
            const float bottom = grayscaleValues[(bottomY * width) + x];
            const float bottomRight = grayscaleValues[(bottomY * width) + rightX];

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

            energyRow[x] = std::sqrt((horizontalGradient * horizontalGradient) +
                                     (verticalGradient * verticalGradient));
        }
    }
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
