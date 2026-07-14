#ifndef SEAMCRAFT_SEAM_REMOVER_HPP
#define SEAMCRAFT_SEAM_REMOVER_HPP

#include "PixelGraph.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

// Responsible for removing a vertical seam from an image.
class SeamRemover
{
public:
    // Removes the specified vertical seam from the source image.
    // Returns a new sf::Image that is one pixel narrower, with the seam pixels removed.
    sf::Image removeSeam(const sf::Image& image,
                         const std::vector<unsigned int>& seamNodeIds,
                         const PixelGraph& graph) const;
};

#endif
