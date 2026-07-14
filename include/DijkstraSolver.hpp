#ifndef SEAMCRAFT_DIJKSTRA_SOLVER_HPP
#define SEAMCRAFT_DIJKSTRA_SOLVER_HPP

#include "PixelGraph.hpp"

#include <vector>

// Runs Dijkstra's algorithm on a PixelGraph to find the minimum-energy vertical seam.
// The seam is returned as an ordered list of node ids from the top row to the bottom row.
class DijkstraSolver
{
public:
    void solve(const PixelGraph& graph);

    const std::vector<unsigned int>& getSeam() const;
    float getTotalEnergy() const;
    bool hasSeam() const;
    bool validateSeam(const PixelGraph& graph) const;

private:
    // The seam stored as node ids ordered from top row (index 0) to bottom row (index height-1).
    std::vector<unsigned int> seam;
    std::vector<float> distance;
    std::vector<unsigned int> predecessor;
    std::vector<unsigned char> visited;
    float totalEnergy = 0.0f;
};

#endif
