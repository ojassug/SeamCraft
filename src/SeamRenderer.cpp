#include "SeamRenderer.hpp"

#include <algorithm>

namespace
{
constexpr float WindowPadding = 40.0f;
}

bool SeamRenderer::updateFromSeam(const std::vector<unsigned int>& seamNodeIds, const PixelGraph& graph)
{
    if (seamNodeIds.empty() || graph.getNodeCount() == 0)
    {
        width = 0;
        height = 0;
        sprite.reset();
        return false;
    }

    width = graph.getImageWidth();
    height = graph.getImageHeight();

    overlayImage.resize({width, height}, sf::Color::Transparent);

    for (unsigned int nodeId : seamNodeIds)
    {
        const auto [x, y] = graph.coordinatesFromNodeId(nodeId);
        if (x < width && y < height)
        {
            overlayImage.setPixel({x, y}, sf::Color::Red);
        }
    }

    if (!texture.loadFromImage(overlayImage))
    {
        sprite.reset();
        return false;
    }

    sprite.emplace(texture);
    scaleFactor = 1.0f;
    return true;
}

bool SeamRenderer::hasOverlay() const
{
    return sprite.has_value();
}

void SeamRenderer::fitToWindow(const sf::Vector2u& windowSize)
{
    if (!hasOverlay() || width == 0 || height == 0)
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

void SeamRenderer::draw(sf::RenderTarget& renderTarget) const
{
    if (hasOverlay())
    {
        renderTarget.draw(*sprite);
    }
}

unsigned int SeamRenderer::getWidth() const
{
    return width;
}

unsigned int SeamRenderer::getHeight() const
{
    return height;
}
