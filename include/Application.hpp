#ifndef SEAMCRAFT_APPLICATION_HPP
#define SEAMCRAFT_APPLICATION_HPP

#include "DijkstraSolver.hpp"
#include "EnergyCalculator.hpp"
#include "EnergyRenderer.hpp"
#include "ImageManager.hpp"
#include "PixelGraph.hpp"
#include "SeamRenderer.hpp"
#include "SeamRemover.hpp"
#include "Window.hpp"

// Owns the high-level application lifecycle and main loop.
class Application
{
public:
    Application();

    void run();

private:
    void loadStartupImage();
    void handleEvent(const sf::Event& event);
    void openImage();
    void resetImage();
    void calculateEnergyMap();
    void updateEnergyVisualization();
    void rebuildPixelGraph();
    void toggleDisplayMode();
    void toggleSeamVisibility();
    void toggleContinuousCarving();
    void stopContinuousCarving();
    void updateContinuousCarving();
    bool removeActiveSeam();
    void computeShortestSeam();
    void printEnergyDebugInfo() const;
    void printGraphDebugInfo() const;
    void printSeamDebugInfo() const;

    // Helper methods to keep removeActiveSeam readable.
    void profileSeamRemoval(const sf::Image& currentImage,
                            const std::vector<unsigned int>& seam,
                            sf::Image& carvedImage,
                            SeamProfilingReport& report);
    bool profileImageUpdateAndEnergyRecalc(sf::Image& carvedImage,
                                           SeamProfilingReport& report);

    struct SeamProfilingReport
    {
        unsigned int inputWidth = 0;
        unsigned int inputHeight = 0;
        unsigned int outputWidth = 0;
        unsigned int outputHeight = 0;
        unsigned int graphNodeCount = 0;
        unsigned int graphEdgeCount = 0;
        long long seamRemovalMilliseconds = 0;
        long long imageManagerUpdateMilliseconds = 0;
        long long energyImageSetupMilliseconds = 0;
        long long energyCalculationMilliseconds = 0;
        long long energyRendererUpdateMilliseconds = 0;
        long long energyDebugOutputMilliseconds = 0;
        long long pixelGraphConstructionMilliseconds = 0;
        long long graphDebugOutputMilliseconds = 0;
        long long dijkstraMilliseconds = 0;
        long long seamDebugOutputMilliseconds = 0;
        long long seamRendererUpdateMilliseconds = 0;
        long long totalProcessingMilliseconds = 0;
    };

    void printSeamProfilingReport(const SeamProfilingReport& report) const;
    void setLoadedStatus();
    void setStatus(const std::string& message);
    void updateWindowTitle();

    Window window;
    ImageManager imageManager;
    EnergyCalculator energyCalculator;
    EnergyRenderer energyRenderer;
    PixelGraph pixelGraph;
    DijkstraSolver dijkstraSolver;
    SeamRenderer seamRenderer;
    SeamRemover seamRemover;
    std::string statusMessage;
    sf::Clock continuousCarvingClock;
    sf::Time continuousCarvingInterval = sf::milliseconds(150);
    bool showingEnergyMap = false;
    bool showingSeam = false;
    bool continuousCarvingEnabled = false;
};

#endif
