#ifndef SEAMCRAFT_PIXEL_GRAPH_HPP
#define SEAMCRAFT_PIXEL_GRAPH_HPP

#include "EnergyCalculator.hpp"

#include <utility>
#include <vector>

struct GraphNode
{
    unsigned int id = 0;
    unsigned int x = 0;
    unsigned int y = 0;
    float energy = 0.0f;
};

struct GraphEdge
{
    unsigned int destinationNodeId = 0;
    float weight = 0.0f;
};

// Builds the vertical seam graph from the current energy map.
class PixelGraph
{
public:
    void build(const EnergyCalculator& energyCalculator);

    const std::vector<GraphEdge>& getNeighbours(unsigned int nodeId) const;
    const GraphNode& getNode(unsigned int nodeId) const;
    unsigned int getNodeCount() const;
    unsigned int getEdgeCount() const;
    unsigned int getImageWidth() const;
    unsigned int getImageHeight() const;

    unsigned int nodeIdFromCoordinates(unsigned int x, unsigned int y) const;
    std::pair<unsigned int, unsigned int> coordinatesFromNodeId(unsigned int nodeId) const;
    bool validateVerticalConnectivity() const;

private:
    void addVerticalEdgesForNode(const GraphNode& node);
    bool isValidCoordinate(int x, int y) const;

    std::vector<GraphNode> nodes;
    std::vector<std::vector<GraphEdge>> adjacencyList;
    unsigned int imageWidth = 0;
    unsigned int imageHeight = 0;
    unsigned int edgeCount = 0;
};

#endif
