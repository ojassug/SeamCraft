#ifndef SEAMCRAFT_APPLICATION_HPP
#define SEAMCRAFT_APPLICATION_HPP

#include "EnergyCalculator.hpp"
#include "EnergyRenderer.hpp"
#include "ImageManager.hpp"
#include "PixelGraph.hpp"
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
    void printEnergyDebugInfo() const;
    void printGraphDebugInfo() const;
    void setLoadedStatus();
    void setStatus(const std::string& message);
    void updateWindowTitle();

    Window window;
    ImageManager imageManager;
    EnergyCalculator energyCalculator;
    EnergyRenderer energyRenderer;
    PixelGraph pixelGraph;
    std::string statusMessage;
    bool showingEnergyMap = false;
};

#endif
