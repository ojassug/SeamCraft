#include "DijkstraSolver.hpp"

#include <limits>
#include <queue>
#include <utility>
#include <vector>

void DijkstraSolver::solve(const PixelGraph& graph)
{
    seam.clear();
    totalEnergy = 0.0f;

    const unsigned int nodeCount = graph.getNodeCount();
    const unsigned int imageWidth = graph.getImageWidth();
    const unsigned int imageHeight = graph.getImageHeight();

    if (nodeCount == 0 || imageWidth == 0 || imageHeight == 0)
    {
        return;
    }

    // Dijkstra's algorithm: find the minimum-energy vertical seam.
    // Edge weight equals destination pixel energy; top-row pixels are seeded
    // with their own energy. Predecessor chain is walked back from best bottom node.

    constexpr float Infinity = std::numeric_limits<float>::max();
    constexpr unsigned int NoPredecessor = std::numeric_limits<unsigned int>::max();

    // Allocate or reuse distance, predecessor, and visited arrays for every node.
    distance.assign(nodeCount, Infinity);
    predecessor.assign(nodeCount, NoPredecessor);
    visited.assign(nodeCount, 0);

    // Min-heap: pairs of (distance, nodeId). Smallest distance on top.
    using QueueEntry = std::pair<float, unsigned int>;
    std::vector<QueueEntry> heapStorage;
    heapStorage.reserve(nodeCount);
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> minHeap(
        std::greater<QueueEntry>(),
        std::move(heapStorage));

    // Seed the priority queue with every pixel in the top row (y == 0).
    // The initial distance for a top-row pixel is its own energy, because
    // the seam must include that pixel.
    for (unsigned int x = 0; x < imageWidth; ++x)
    {
        const unsigned int startNodeId = x;
        const float startEnergy = graph.getNode(startNodeId).energy;
        distance[startNodeId] = startEnergy;
        minHeap.push({startEnergy, startNodeId});
    }

    // Main Dijkstra loop: settle nodes in order of increasing distance.
    while (!minHeap.empty())
    {
        // Extract the unvisited node with the smallest tentative distance.
        const auto [currentDistance, currentNodeId] = minHeap.top();
        minHeap.pop();

        // Skip this entry if we have already settled this node. This handles
        // duplicate entries that were pushed when we found a shorter path.
        if (visited[currentNodeId])
        {
            continue;
        }

        visited[currentNodeId] = 1;

        // Relax every outgoing edge from the current node.
        for (const GraphEdge& edge : graph.getNeighbours(currentNodeId))
        {
            const unsigned int neighbourId = edge.destinationNodeId;
            const float newDistance = currentDistance + edge.weight;

            // If we found a shorter path to this neighbour, update it.
            if (newDistance < distance[neighbourId])
            {
                distance[neighbourId] = newDistance;
                predecessor[neighbourId] = currentNodeId;
                minHeap.push({newDistance, neighbourId});
            }
        }
    }

    // -------------------------------------------------------------------------
    // Find the bottom-row pixel with the smallest total distance.
    // The bottom row is at y == imageHeight - 1.
    // -------------------------------------------------------------------------
    const unsigned int bottomRow = imageHeight - 1;
    const unsigned int bottomRowStart = bottomRow * imageWidth;
    unsigned int bestBottomNodeId = 0;
    float bestBottomDistance = Infinity;

    for (unsigned int x = 0; x < imageWidth; ++x)
    {
        const unsigned int bottomNodeId = bottomRowStart + x;
        if (distance[bottomNodeId] < bestBottomDistance)
        {
            bestBottomDistance = distance[bottomNodeId];
            bestBottomNodeId = bottomNodeId;
        }
    }

    // If no bottom-row pixel was reached, the graph is disconnected.
    if (bestBottomDistance == Infinity)
    {
        return;
    }

    totalEnergy = bestBottomDistance;

    // -------------------------------------------------------------------------
    // Reconstruct the seam by walking the predecessor chain from the best
    // bottom-row pixel back to a top-row pixel.
    // -------------------------------------------------------------------------
    seam.resize(imageHeight);

    unsigned int currentNodeId = bestBottomNodeId;
    for (int row = static_cast<int>(imageHeight) - 1; row >= 0; --row)
    {
        seam[static_cast<unsigned int>(row)] = currentNodeId;
        currentNodeId = predecessor[currentNodeId];
    }
}

const std::vector<unsigned int>& DijkstraSolver::getSeam() const
{
    return seam;
}

float DijkstraSolver::getTotalEnergy() const
{
    return totalEnergy;
}

bool DijkstraSolver::hasSeam() const
{
    return !seam.empty();
}

bool DijkstraSolver::validateSeam(const PixelGraph& graph) const
{
    if (seam.empty())
    {
        return false;
    }

    const unsigned int imageHeight = graph.getImageHeight();

    // Check 1: Seam length must equal image height.
    if (seam.size() != imageHeight)
    {
        return false;
    }

    // Check 2: First node must be in the top row (y == 0).
    const auto [firstX, firstY] = graph.coordinatesFromNodeId(seam.front());
    if (firstY != 0)
    {
        return false;
    }

    // Check 3: Last node must be in the bottom row (y == imageHeight - 1).
    const auto [lastX, lastY] = graph.coordinatesFromNodeId(seam.back());
    if (lastY != imageHeight - 1)
    {
        return false;
    }

    // Check 4: Each consecutive pair of nodes must be in adjacent rows, and
    //           their columns must differ by at most one.
    for (unsigned int i = 1; i < seam.size(); ++i)
    {
        const auto [prevX, prevY] = graph.coordinatesFromNodeId(seam[i - 1]);
        const auto [currX, currY] = graph.coordinatesFromNodeId(seam[i]);

        // Must move exactly one row down.
        if (currY != prevY + 1)
        {
            return false;
        }

        // Column shift must be -1, 0, or +1.
        const int columnDifference = static_cast<int>(currX) - static_cast<int>(prevX);
        if (columnDifference < -1 || columnDifference > 1)
        {
            return false;
        }
    }

    return true;
}
