#include "SeamRemover.hpp"
#include <stdexcept>

sf::Image SeamRemover::removeSeam(const sf::Image& image,
                                  const std::vector<unsigned int>& seamNodeIds,
                                  const PixelGraph& graph) const
{
    const sf::Vector2u imageSize = image.getSize();
    const unsigned int oldWidth = imageSize.x;
    const unsigned int height = imageSize.y;

    if (oldWidth <= 1)
    {
        throw std::invalid_argument("Cannot remove a seam from an image of width 1 or less.");
    }
    if (seamNodeIds.size() != height)
    {
        throw std::invalid_argument("Seam size does not match the image height.");
    }

    sf::Image newImage;
    newImage.resize({oldWidth - 1, height});

    // Process row by row to shift the pixels after the seam to the left.
    for (unsigned int y = 0; y < height; ++y)
    {
        const auto [seamX, seamY] = graph.coordinatesFromNodeId(seamNodeIds[y]);

        if (seamY != y)
        {
            throw std::runtime_error("Seam node Y coordinate does not match row index y.");
        }
        if (seamX >= oldWidth)
        {
            throw std::runtime_error("Seam node X coordinate is out of image bounds.");
        }

        // Copy pixels to the left of the seam
        for (unsigned int x = 0; x < seamX; ++x)
        {
            newImage.setPixel({x, y}, image.getPixel({x, y}));
        }

        // Copy pixels to the right of the seam, shifting them left by one pixel
        for (unsigned int x = seamX; x < oldWidth - 1; ++x)
        {
            newImage.setPixel({x, y}, image.getPixel({x + 1, y}));
        }
    }

    return newImage;
}
