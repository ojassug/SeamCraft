#include "Application.hpp"

#include "tinyfiledialogs.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace
{
constexpr unsigned int WindowWidth = 1200;
constexpr unsigned int WindowHeight = 800;
constexpr const char WindowTitle[] = "SeamCraft";
constexpr const char StartupImagePath[] = "assets/images/sample.png";
constexpr const char TitleHelpText[] = " | O: Open | R: Reset | E: Energy | S: Seam | C: Carve | P: Save | Space: Auto";
constexpr const char OutputDirectory[] = "assets/output";
constexpr const char CarvedFilePrefix[] = "carved_";
constexpr const char CarvedFileExtension[] = ".png";

using ProfilingClock = std::chrono::high_resolution_clock;

long long elapsedMilliseconds(const ProfilingClock::time_point& startTime,
                              const ProfilingClock::time_point& endTime)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}
}

Application::Application()
    : window(WindowWidth, WindowHeight, WindowTitle)
{
    loadStartupImage();
    updateWindowTitle();
}

void Application::run()
{
    while (window.isOpen())
    {
        window.handleEvents([this](const sf::Event& event) {
            handleEvent(event);
        });

        updateContinuousCarving();

        const sf::Vector2u windowSize = window.getRenderWindow().getSize();
        if (showingEnergyMap)
        {
            energyRenderer.fitToWindow(windowSize);
        }
        else
        {
            imageManager.fitToWindow(windowSize);
        }

        if (showingSeam)
        {
            seamRenderer.fitToWindow(windowSize);
        }

        window.beginFrame();
        if (showingEnergyMap)
        {
            energyRenderer.draw(window.getRenderWindow());
        }
        else
        {
            imageManager.draw(window.getRenderWindow());
        }

        if (showingSeam)
        {
            seamRenderer.draw(window.getRenderWindow());
        }
        window.endFrame();
    }
}

void Application::loadStartupImage()
{
    if (imageManager.loadImage(StartupImagePath))
    {
        calculateEnergyMap();
        setLoadedStatus();
    }
    else
    {
        setStatus("Startup image not loaded: " + imageManager.getLastError());
    }
}

void Application::handleEvent(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (keyPressed == nullptr)
    {
        return;
    }

    if (keyPressed->code == sf::Keyboard::Key::O)
    {
        openImage();
    }
    else if (keyPressed->code == sf::Keyboard::Key::R)
    {
        resetImage();
    }
    else if (keyPressed->code == sf::Keyboard::Key::E)
    {
        toggleDisplayMode();
    }
    else if (keyPressed->code == sf::Keyboard::Key::S)
    {
        toggleSeamVisibility();
    }
    else if (keyPressed->code == sf::Keyboard::Key::C)
    {
        removeActiveSeam();
    }
    else if (keyPressed->code == sf::Keyboard::Key::P)
    {
        saveCarvedImage();
    }
    else if (keyPressed->code == sf::Keyboard::Key::Space)
    {
        toggleContinuousCarving();
    }
}

void Application::openImage()
{
    stopContinuousCarving();

    const char* filterPatterns[] = {"*.png", "*.jpg", "*.jpeg"};
    const char* selectedPath = tinyfd_openFileDialog(
        "Open Image",
        "",
        3,
        filterPatterns,
        "Image files (*.png, *.jpg, *.jpeg)",
        0);

    if (selectedPath == nullptr)
    {
        setStatus("Open canceled.");
        return;
    }

    if (imageManager.loadImage(selectedPath))
    {
        calculateEnergyMap();
        setLoadedStatus();
    }
    else
    {
        setStatus("Load failed: " + imageManager.getLastError());
    }
}

void Application::resetImage()
{
    stopContinuousCarving();

    if (imageManager.resetImage())
    {
        calculateEnergyMap();
        setStatus("Image reset.");
    }
    else
    {
        setStatus("Reset failed: " + imageManager.getLastError());
    }
}

void Application::calculateEnergyMap()
{
    energyCalculator.setImage(imageManager.getCurrentImage());
    energyCalculator.calculate();
    updateEnergyVisualization();
    rebuildPixelGraph();
}

void Application::updateEnergyVisualization()
{
    if (energyRenderer.updateFromEnergyMap(energyCalculator.getEnergyMap()))
    {
        showingEnergyMap = showingEnergyMap; // unchanged
    }
    else
    {
        showingEnergyMap = false;
    }
}

void Application::rebuildPixelGraph()
{
    pixelGraph.build(energyCalculator);
    computeShortestSeam();
}

void Application::toggleDisplayMode()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image loaded.");
        return;
    }

    if (!energyRenderer.hasVisualization())
    {
        setStatus("Energy view unavailable.");
        return;
    }

    showingEnergyMap = !showingEnergyMap;
    updateWindowTitle();
}

void Application::toggleSeamVisibility()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image loaded.");
        return;
    }

    if (!seamRenderer.hasOverlay())
    {
        setStatus("Seam overlay unavailable.");
        return;
    }

    showingSeam = !showingSeam;
    updateWindowTitle();
}

void Application::toggleContinuousCarving()
{
    if (continuousCarvingEnabled)
    {
        stopContinuousCarving();
        return;
    }

    if (!imageManager.hasImage())
    {
        setStatus("No image loaded.");
        return;
    }

    if (!dijkstraSolver.hasSeam())
    {
        setStatus("No seam to carve.");
        return;
    }

    continuousCarvingEnabled = true;
    continuousCarvingClock.restart();
    setStatus("Auto-carve started.");
}

void Application::stopContinuousCarving()
{
    if (!continuousCarvingEnabled)
    {
        return;
    }

    continuousCarvingEnabled = false;
    setStatus("Auto-carve stopped.");
}

void Application::updateContinuousCarving()
{
    if (!continuousCarvingEnabled)
    {
        return;
    }

    if (continuousCarvingClock.getElapsedTime() < continuousCarvingInterval)
    {
        return;
    }

    continuousCarvingClock.restart();

    if (!removeActiveSeam())
    {
        continuousCarvingEnabled = false;
        updateWindowTitle();
    }
}

void Application::profileSeamRemoval(const sf::Image& currentImage,
                                     const std::vector<unsigned int>& seam,
                                     sf::Image& carvedImage,
                                     SeamProfilingReport& report)
{
    const auto seamRemovalStartTime = ProfilingClock::now();
    carvedImage = seamRemover.removeSeam(currentImage, seam, pixelGraph);
    const auto seamRemovalEndTime = ProfilingClock::now();
    report.seamRemovalMilliseconds = elapsedMilliseconds(seamRemovalStartTime, seamRemovalEndTime);
}

bool Application::profileImageUpdateAndEnergyRecalc(sf::Image& carvedImage,
                                                   SeamProfilingReport& report)
{
    const auto imageManagerUpdateStartTime = ProfilingClock::now();
    if (!imageManager.setCurrentImage(carvedImage))
    {
        const auto imageManagerUpdateEndTime = ProfilingClock::now();
        report.imageManagerUpdateMilliseconds =
            elapsedMilliseconds(imageManagerUpdateStartTime, imageManagerUpdateEndTime);
        setStatus("Update failed: " + imageManager.getLastError());
        return false;
    }
    const auto imageManagerUpdateEndTime = ProfilingClock::now();
    report.imageManagerUpdateMilliseconds =
        elapsedMilliseconds(imageManagerUpdateStartTime, imageManagerUpdateEndTime);
    report.outputWidth = imageManager.getWidth();
    report.outputHeight = imageManager.getHeight();

    const auto energyImageSetupStartTime = ProfilingClock::now();
    energyCalculator.setImage(imageManager.getCurrentImage());
    const auto energyImageSetupEndTime = ProfilingClock::now();
    report.energyImageSetupMilliseconds =
        elapsedMilliseconds(energyImageSetupStartTime, energyImageSetupEndTime);

    const auto energyCalculationStartTime = ProfilingClock::now();
    energyCalculator.calculate();
    const auto energyCalculationEndTime = ProfilingClock::now();
    report.energyCalculationMilliseconds =
        elapsedMilliseconds(energyCalculationStartTime, energyCalculationEndTime);

    const auto energyRendererUpdateStartTime = ProfilingClock::now();
    updateEnergyVisualization();
    const auto energyRendererUpdateEndTime = ProfilingClock::now();
    report.energyRendererUpdateMilliseconds =
        elapsedMilliseconds(energyRendererUpdateStartTime, energyRendererUpdateEndTime);

    const auto pixelGraphConstructionStartTime = ProfilingClock::now();
    pixelGraph.build(energyCalculator);
    const auto pixelGraphConstructionEndTime = ProfilingClock::now();
    report.pixelGraphConstructionMilliseconds =
        elapsedMilliseconds(pixelGraphConstructionStartTime, pixelGraphConstructionEndTime);
    report.graphNodeCount = pixelGraph.getNodeCount();
    report.graphEdgeCount = pixelGraph.getEdgeCount();

    const auto dijkstraStartTime = ProfilingClock::now();
    dijkstraSolver.solve(pixelGraph);
    const auto dijkstraEndTime = ProfilingClock::now();
    report.dijkstraMilliseconds = elapsedMilliseconds(dijkstraStartTime, dijkstraEndTime);

    const auto seamRendererUpdateStartTime = ProfilingClock::now();
    seamRenderer.updateFromSeam(dijkstraSolver.getSeam(), pixelGraph);
    const auto seamRendererUpdateEndTime = ProfilingClock::now();
    report.seamRendererUpdateMilliseconds =
        elapsedMilliseconds(seamRendererUpdateStartTime, seamRendererUpdateEndTime);

    return true;
}

bool Application::removeActiveSeam()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image loaded.");
        return false;
    }

    if (!dijkstraSolver.hasSeam())
    {
        setStatus("No seam to carve.");
        return false;
    }

    try
    {
        SeamProfilingReport profilingReport;
        const sf::Image& currentImage = imageManager.getCurrentImage();
        const std::vector<unsigned int>& seam = dijkstraSolver.getSeam();
        const sf::Vector2u inputImageSize = currentImage.getSize();

        profilingReport.inputWidth = inputImageSize.x;
        profilingReport.inputHeight = inputImageSize.y;

        const auto totalStartTime = ProfilingClock::now();

        sf::Image carvedImage;
        profileSeamRemoval(currentImage, seam, carvedImage, profilingReport);

        if (!profileImageUpdateAndEnergyRecalc(carvedImage, profilingReport))
        {
            return false;
        }

        std::ostringstream status;
        status << "Carved: " << imageManager.getWidth() << "x" << imageManager.getHeight();
        setStatus(status.str());

        const auto totalEndTime = ProfilingClock::now();
        profilingReport.totalProcessingMilliseconds = elapsedMilliseconds(totalStartTime, totalEndTime);
        return true;
    }
    catch (const std::exception& e)
    {
        setStatus("Carve error: " + std::string(e.what()));
        return false;
    }
}

void Application::saveCarvedImage()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image loaded.");
        return;
    }

    std::error_code dirError;
    if (!std::filesystem::exists(OutputDirectory, dirError))
    {
        if (!std::filesystem::create_directories(OutputDirectory, dirError))
        {
            setStatus("Cannot create output dir: " + dirError.message());
            return;
        }
    }

    int nextIndex = 1;
    std::string candidatePath;
    do
    {
        std::ostringstream fileName;
        fileName << CarvedFilePrefix
                 << std::setw(3) << std::setfill('0') << nextIndex
                 << CarvedFileExtension;
        candidatePath = (std::filesystem::path(OutputDirectory) / fileName.str()).string();
        ++nextIndex;
    } while (std::filesystem::exists(candidatePath, dirError));

    if (imageManager.saveCurrentImage(candidatePath))
    {
        setStatus("Saved: " + candidatePath);
    }
    else
    {
        setStatus("Save failed: " + imageManager.getLastError());
    }
}

void Application::printSeamProfilingReport(const SeamProfilingReport& report) const
{
    std::cout << "\n--- SeamCraft Profiling ---\n";
    std::cout << "Image: " << report.inputWidth << "x" << report.inputHeight
              << " -> " << report.outputWidth << "x" << report.outputHeight << '\n';
    std::cout << "Nodes: " << report.graphNodeCount << " Edges: " << report.graphEdgeCount << '\n';
    std::cout << "Seam removal: " << report.seamRemovalMilliseconds << " ms\n";
    std::cout << "Image update: " << report.imageManagerUpdateMilliseconds << " ms\n";
    std::cout << "Energy calc: " << report.energyCalculationMilliseconds << " ms\n";
    std::cout << "Graph build: " << report.pixelGraphConstructionMilliseconds << " ms\n";
    std::cout << "Dijkstra: " << report.dijkstraMilliseconds << " ms\n";
    std::cout << "Total: " << report.totalProcessingMilliseconds << " ms\n\n";
}

void Application::printEnergyDebugInfo() const
{
    const EnergyCalculator::EnergyMap& energyMap = energyCalculator.getEnergyMap();
    if (energyMap.empty() || energyMap.front().empty())
    {
        return;
    }

    float minimumEnergy = std::numeric_limits<float>::max();
    float maximumEnergy = std::numeric_limits<float>::lowest();
    double totalEnergy = 0.0;
    unsigned int energyCount = 0;

    for (const std::vector<float>& row : energyMap)
    {
        for (float energy : row)
        {
            minimumEnergy = std::min(minimumEnergy, energy);
            maximumEnergy = std::max(maximumEnergy, energy);
            totalEnergy += static_cast<double>(energy);
            ++energyCount;
        }
    }

    const double averageEnergy = totalEnergy / static_cast<double>(energyCount);
    std::cout << "Energy: min=" << minimumEnergy << " max=" << maximumEnergy
              << " avg=" << averageEnergy << '\n';
}

void Application::printGraphDebugInfo() const
{
    std::cout << "Graph: " << pixelGraph.getNodeCount() << " nodes, "
              << pixelGraph.getEdgeCount() << " edges\n";
}

void Application::computeShortestSeam()
{
    dijkstraSolver.solve(pixelGraph);
    seamRenderer.updateFromSeam(dijkstraSolver.getSeam(), pixelGraph);
}

void Application::printSeamDebugInfo() const
{
    if (!dijkstraSolver.hasSeam())
    {
        return;
    }

    std::cout << "Seam: length=" << dijkstraSolver.getSeam().size()
              << " energy=" << dijkstraSolver.getTotalEnergy() << '\n';
}

void Application::setLoadedStatus()
{
    std::ostringstream status;
    status << "Loaded " << imageManager.getFilename()
           << " (" << imageManager.getWidth() << "x" << imageManager.getHeight() << ")";
    setStatus(status.str());
}

void Application::setStatus(const std::string& message)
{
    statusMessage = message;
    std::cout << message << '\n';
    updateWindowTitle();
}

void Application::updateWindowTitle()
{
    std::string title = WindowTitle;
    title += showingEnergyMap ? " | Energy" : " | Image";

    if (showingSeam)
    {
        title += " +Seam";
    }

    if (continuousCarvingEnabled)
    {
        title += " | Auto";
    }

    if (!statusMessage.empty())
    {
        title += " | " + statusMessage;
    }

    title += TitleHelpText;
    window.getRenderWindow().setTitle(title);
}
