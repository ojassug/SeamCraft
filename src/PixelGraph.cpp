#include "PixelGraph.hpp"

#include <stdexcept>

void PixelGraph::build(const EnergyCalculator& energyCalculator)
{
    imageWidth = energyCalculator.getWidth();
    imageHeight = energyCalculator.getHeight();
    edgeCount = 0;

    const unsigned int totalNodeCount = imageWidth * imageHeight;
    nodes.clear();
    adjacencyList.clear();
    nodes.reserve(totalNodeCount);
    adjacencyList.resize(totalNodeCount);

    for (unsigned int y = 0; y < imageHeight; ++y)
    {
        for (unsigned int x = 0; x < imageWidth; ++x)
        {
            const unsigned int nodeId = nodeIdFromCoordinates(x, y);
            nodes.push_back({nodeId, x, y, energyCalculator.getEnergy(x, y)});
        }
    }

    for (const GraphNode& node : nodes)
    {
        addVerticalEdgesForNode(node);
    }
}

const std::vector<GraphEdge>& PixelGraph::getNeighbours(unsigned int nodeId) const
{
    if (nodeId >= adjacencyList.size())
    {
        throw std::out_of_range("Node id is outside the adjacency list.");
    }

    return adjacencyList[nodeId];
}

const GraphNode& PixelGraph::getNode(unsigned int nodeId) const
{
    if (nodeId >= nodes.size())
    {
        throw std::out_of_range("Node id is outside the node list.");
    }

    return nodes[nodeId];
}

unsigned int PixelGraph::getNodeCount() const
{
    return static_cast<unsigned int>(nodes.size());
}

unsigned int PixelGraph::getEdgeCount() const
{
    return edgeCount;
}

unsigned int PixelGraph::getImageWidth() const
{
    return imageWidth;
}

unsigned int PixelGraph::getImageHeight() const
{
    return imageHeight;
}

unsigned int PixelGraph::nodeIdFromCoordinates(unsigned int x, unsigned int y) const
{
    if (x >= imageWidth || y >= imageHeight)
    {
        throw std::out_of_range("Pixel coordinate is outside the graph image bounds.");
    }

    return (y * imageWidth) + x;
}

std::pair<unsigned int, unsigned int> PixelGraph::coordinatesFromNodeId(unsigned int nodeId) const
{
    if (nodeId >= nodes.size() || imageWidth == 0)
    {
        throw std::out_of_range("Node id is outside the graph image bounds.");
    }

    return {nodeId % imageWidth, nodeId / imageWidth};
}

bool PixelGraph::validateVerticalConnectivity() const
{
    if (imageWidth == 0 || imageHeight == 0 || nodes.empty())
    {
        return false;
    }

    for (const GraphNode& node : nodes)
    {
        unsigned int expectedNeighbourCount = 0;
        if (node.y + 1 < imageHeight)
        {
            for (int xOffset = -1; xOffset <= 1; ++xOffset)
            {
                if (isValidCoordinate(static_cast<int>(node.x) + xOffset, static_cast<int>(node.y) + 1))
                {
                    ++expectedNeighbourCount;
                }
            }
        }

        if (adjacencyList[node.id].size() != expectedNeighbourCount)
        {
            return false;
        }

        for (const GraphEdge& edge : adjacencyList[node.id])
        {
            const GraphNode& neighbour = nodes[edge.destinationNodeId];
            const bool goesToNextRow = neighbour.y == node.y + 1;
            const int xDifference = static_cast<int>(neighbour.x) - static_cast<int>(node.x);
            const bool staysWithinVerticalSeamMoves = xDifference >= -1 && xDifference <= 1;

            if (!goesToNextRow || !staysWithinVerticalSeamMoves || edge.weight != neighbour.energy)
            {
                return false;
            }
        }
    }

    return true;
}

void PixelGraph::addVerticalEdgesForNode(const GraphNode& node)
{
    // A vertical seam moves downward one row at a time, so each pixel only connects
    // to the valid down-left, down, and down-right pixels in the next row.
    const int nextY = static_cast<int>(node.y) + 1;
    for (int xOffset = -1; xOffset <= 1; ++xOffset)
    {
        const int nextX = static_cast<int>(node.x) + xOffset;
        if (!isValidCoordinate(nextX, nextY))
        {
            continue;
        }

        const unsigned int neighbourId = nodeIdFromCoordinates(
            static_cast<unsigned int>(nextX),
            static_cast<unsigned int>(nextY));

        adjacencyList[node.id].push_back({neighbourId, nodes[neighbourId].energy});
        ++edgeCount;
    }
}

bool PixelGraph::isValidCoordinate(int x, int y) const
{
    return x >= 0 &&
           y >= 0 &&
           x < static_cast<int>(imageWidth) &&
           y < static_cast<int>(imageHeight);
}
