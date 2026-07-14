#include "PixelGraph.hpp"

#include <stdexcept>

void PixelGraph::build(const EnergyCalculator& energyCalculator)
{
    imageWidth = energyCalculator.getWidth();
    imageHeight = energyCalculator.getHeight();
    edgeCount = 0;

    const unsigned int totalNodeCount = imageWidth * imageHeight;
    nodes.resize(totalNodeCount);
    adjacencyList.reserve(totalNodeCount);
    adjacencyList.resize(totalNodeCount);

    const EnergyCalculator::EnergyMap& energyMap = energyCalculator.getEnergyMap();

    for (unsigned int y = 0; y < imageHeight; ++y)
    {
        const std::vector<float>& energyRow = energyMap[y];
        for (unsigned int x = 0; x < imageWidth; ++x)
        {
            const unsigned int nodeId = (y * imageWidth) + x;
            nodes[nodeId] = {nodeId, x, y, energyRow[x]};

            std::vector<GraphEdge>& edges = adjacencyList[nodeId];
            edges.clear();

            if (y + 1 >= imageHeight)
            {
                continue;
            }

            const unsigned int nextRow = y + 1;
            const unsigned int nextRowStart = nextRow * imageWidth;
            const std::vector<float>& nextEnergyRow = energyMap[nextRow];
            const unsigned int firstNextX = x == 0 ? 0 : x - 1;
            const unsigned int lastNextX = (x + 1 >= imageWidth) ? imageWidth - 1 : x + 1;
            const unsigned int expectedEdgeCount = (lastNextX - firstNextX) + 1;

            if (edges.capacity() < expectedEdgeCount)
            {
                edges.reserve(expectedEdgeCount);
            }

            for (unsigned int nextX = firstNextX; nextX <= lastNextX; ++nextX)
            {
                const unsigned int neighbourId = nextRowStart + nextX;
                edges.push_back({neighbourId, nextEnergyRow[nextX]});
                ++edgeCount;
            }
        }
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

bool PixelGraph::isValidCoordinate(int x, int y) const
{
    return x >= 0 &&
           y >= 0 &&
           x < static_cast<int>(imageWidth) &&
           y < static_cast<int>(imageHeight);
}
