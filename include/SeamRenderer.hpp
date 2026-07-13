#ifndef SEAMCRAFT_SEAM_RENDERER_HPP
#define SEAMCRAFT_SEAM_RENDERER_HPP

#include "PixelGraph.hpp"

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

// Draws the computed seam as a coloured overlay on top of the original image.
// The renderer does NOT compute the seam. It only visualizes an existing one.
class SeamRenderer
{
public:
    bool updateFromSeam(const std::vector<unsigned int>& seamNodeIds,
                        const PixelGraph& graph);

    bool hasOverlay() const;
    void fitToWindow(const sf::Vector2u& windowSize);
    void draw(sf::RenderTarget& renderTarget) const;

    unsigned int getWidth() const;
    unsigned int getHeight() const;

private:
    sf::Image overlayImage;
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;
    unsigned int width = 0;
    unsigned int height = 0;
    float scaleFactor = 1.0f;
};

#endif
