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
constexpr const char TitleHelpText[] = " | O: Open Image | R: Reset | E: Toggle Energy | S: Toggle Seam | C: Carve | P: Save | Space: Auto Carve";
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
    // Main loop: process input, prepare the frame, draw, then present it.
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
        std::cout << "Loaded startup image: " << imageManager.getFilename() << '\n';
        calculateEnergyMap();
        setLoadedStatus();
    }
    else
    {
        std::cout << "Startup image was not loaded. " << imageManager.getLastError() << '\n';
        std::cout << "The application will continue without an image.\n";
        setStatus("Startup image was not loaded. " + imageManager.getLastError());
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
        std::cout << "========== P KEY PRESSED ==========\n";
        setStatus("P key detected");
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
        setStatus("Open image canceled.");
        return;
    }

    if (imageManager.loadImage(selectedPath))
    {
        calculateEnergyMap();
        setLoadedStatus();
    }
    else
    {
        setStatus("Failed to load image. " + imageManager.getLastError());
    }
}

void Application::resetImage()
{
    stopContinuousCarving();

    if (imageManager.resetImage())
    {
        calculateEnergyMap();
        setStatus("Reset image: " + imageManager.getFilename());
    }
    else
    {
        setStatus("Reset failed. " + imageManager.getLastError());
    }
}

void Application::calculateEnergyMap()
{
    energyCalculator.setImage(imageManager.getCurrentImage());
    energyCalculator.calculate();
    updateEnergyVisualization();
    rebuildPixelGraph();
    printEnergyDebugInfo();
}

void Application::updateEnergyVisualization()
{
    if (energyRenderer.updateFromEnergyMap(energyCalculator.getEnergyMap()))
    {
        std::cout << "Energy visualization updated: "
                  << energyRenderer.getWidth() << " x " << energyRenderer.getHeight() << '\n';
    }
    else
    {
        showingEnergyMap = false;
        std::cout << "Energy visualization was not updated.\n";
    }
}

void Application::rebuildPixelGraph()
{
    pixelGraph.build(energyCalculator);
    printGraphDebugInfo();
    computeShortestSeam();
}

void Application::toggleDisplayMode()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image is loaded.");
        return;
    }

    if (!energyRenderer.hasVisualization())
    {
        setStatus("Energy visualization is not available.");
        return;
    }

    showingEnergyMap = !showingEnergyMap;
    updateWindowTitle();
}

void Application::toggleSeamVisibility()
{
    if (!imageManager.hasImage())
    {
        setStatus("No image is loaded.");
        return;
    }

    if (!seamRenderer.hasOverlay())
    {
        setStatus("Seam visualization is not available.");
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
        setStatus("No image is loaded.");
        return;
    }

    if (!dijkstraSolver.hasSeam())
    {
        setStatus("No seam available to carve.");
        return;
    }

    continuousCarvingEnabled = true;
    continuousCarvingClock.restart();
    setStatus("Continuous carving started.");
}

void Application::stopContinuousCarving()
{
    if (!continuousCarvingEnabled)
    {
        return;
    }

    continuousCarvingEnabled = false;
    setStatus("Continuous carving stopped.");
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
        setStatus("Failed to update image after carving. " + imageManager.getLastError());
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

    const auto energyDebugOutputStartTime = ProfilingClock::now();
    printEnergyDebugInfo();
    const auto energyDebugOutputEndTime = ProfilingClock::now();
    report.energyDebugOutputMilliseconds =
        elapsedMilliseconds(energyDebugOutputStartTime, energyDebugOutputEndTime);

    const auto pixelGraphConstructionStartTime = ProfilingClock::now();
    pixelGraph.build(energyCalculator);
    const auto pixelGraphConstructionEndTime = ProfilingClock::now();
    report.pixelGraphConstructionMilliseconds =
        elapsedMilliseconds(pixelGraphConstructionStartTime, pixelGraphConstructionEndTime);
    report.graphNodeCount = pixelGraph.getNodeCount();
    report.graphEdgeCount = pixelGraph.getEdgeCount();

    const auto graphDebugOutputStartTime = ProfilingClock::now();
    printGraphDebugInfo();
    const auto graphDebugOutputEndTime = ProfilingClock::now();
    report.graphDebugOutputMilliseconds =
        elapsedMilliseconds(graphDebugOutputStartTime, graphDebugOutputEndTime);

    const auto dijkstraStartTime = ProfilingClock::now();
    dijkstraSolver.solve(pixelGraph);
    const auto dijkstraEndTime = ProfilingClock::now();
    report.dijkstraMilliseconds = elapsedMilliseconds(dijkstraStartTime, dijkstraEndTime);

    const auto seamDebugOutputStartTime = ProfilingClock::now();
    printSeamDebugInfo();
    const auto seamDebugOutputEndTime = ProfilingClock::now();
    report.seamDebugOutputMilliseconds =
        elapsedMilliseconds(seamDebugOutputStartTime, seamDebugOutputEndTime);

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
        setStatus("No image is loaded.");
        return false;
    }

    if (!dijkstraSolver.hasSeam())
    {
        setStatus("No seam available to carve.");
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
        status << "Carved seam: " << imageManager.getWidth() << " x " << imageManager.getHeight();
        setStatus(status.str());

        const auto totalEndTime = ProfilingClock::now();
        profilingReport.totalProcessingMilliseconds = elapsedMilliseconds(totalStartTime, totalEndTime);
        printSeamProfilingReport(profilingReport);
        return true;
    }
    catch (const std::exception& e)
    {
        setStatus("Carving error: " + std::string(e.what()));
        return false;
    }
}

void Application::saveCarvedImage()
{
    std::cout << "[1] Entered saveCarvedImage\n";
    std::cout << "[2] hasImage = " << (imageManager.hasImage() ? "true" : "false") << "\n";

    if (!imageManager.hasImage())
    {
        setStatus("No image is loaded.");
        return;
    }

    std::error_code dirError;
    if (!std::filesystem::exists(OutputDirectory, dirError))
    {
        std::cout << "[3] Output directory exists\n";
        if (!std::filesystem::create_directories(OutputDirectory, dirError))
        {
            setStatus("Could not create output directory: " + std::string(OutputDirectory));
            return;
        }
        std::cout << "[4] Output directory created\n";
    }
    else
    {
        std::cout << "[3] Output directory exists\n";
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

    std::cout << "[5] Candidate path = " << candidatePath << "\n";
    std::cout << "[6] Calling saveCurrentImage\n";
    if (imageManager.saveCurrentImage(candidatePath))
    {
        setStatus("Saved carved image: " + candidatePath);
    }
    else
    {
        setStatus("Failed to save image. " + imageManager.getLastError());
    }
}

void Application::printSeamProfilingReport(const SeamProfilingReport& report) const
{
    std::cout << "\n========== SeamCraft Profiling Report ==========\n";
    std::cout << "Image resolution: "
              << report.inputWidth << " x " << report.inputHeight
              << " -> " << report.outputWidth << " x " << report.outputHeight << '\n';
    std::cout << "Graph nodes: " << report.graphNodeCount << '\n';
    std::cout << "Graph edges: " << report.graphEdgeCount << '\n';
    std::cout << "Timings (milliseconds):\n";
    std::cout << "  Seam removal: " << report.seamRemovalMilliseconds << '\n';
    std::cout << "  ImageManager update: " << report.imageManagerUpdateMilliseconds << '\n';
    std::cout << "  Energy image setup/copy: " << report.energyImageSetupMilliseconds << '\n';
    std::cout << "  Energy calculation: " << report.energyCalculationMilliseconds << '\n';
    std::cout << "  EnergyRenderer update: " << report.energyRendererUpdateMilliseconds << '\n';
    std::cout << "  Energy debug statistics/output: " << report.energyDebugOutputMilliseconds << '\n';
    std::cout << "  PixelGraph construction: " << report.pixelGraphConstructionMilliseconds << '\n';
    std::cout << "  PixelGraph debug validation/output: " << report.graphDebugOutputMilliseconds << '\n';
    std::cout << "  Dijkstra shortest-path computation: " << report.dijkstraMilliseconds << '\n';
    std::cout << "  Seam debug validation/output: " << report.seamDebugOutputMilliseconds << '\n';
    std::cout << "  SeamRenderer update: " << report.seamRendererUpdateMilliseconds << '\n';
    std::cout << "  Total processing time: " << report.totalProcessingMilliseconds << '\n';
    std::cout << "================================================\n\n";
}

void Application::printEnergyDebugInfo() const
{
    const EnergyCalculator::EnergyMap& energyMap = energyCalculator.getEnergyMap();

    std::cout << "Energy calculation complete.\n";
    std::cout << "Image size: " << imageManager.getWidth() << " x " << imageManager.getHeight() << '\n';
    std::cout << "Energy map size: " << energyCalculator.getWidth() << " x " << energyCalculator.getHeight() << '\n';

    if (energyMap.empty() || energyMap.front().empty())
    {
        std::cout << "Energy statistics are unavailable for an empty image.\n";
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
    std::cout << std::fixed << std::setprecision(2)
              << "Minimum energy: " << minimumEnergy << '\n'
              << "Maximum energy: " << maximumEnergy << '\n'
              << "Average energy: " << averageEnergy << '\n'
              << std::defaultfloat;
}

void Application::printGraphDebugInfo() const
{
    const unsigned int nodeCount = pixelGraph.getNodeCount();
    const unsigned int edgeCount = pixelGraph.getEdgeCount();
    const double averageOutgoingDegree = nodeCount == 0
                                             ? 0.0
                                             : static_cast<double>(edgeCount) / static_cast<double>(nodeCount);

    std::cout << "Pixel graph rebuilt.\n";
    std::cout << "Graph image size: " << pixelGraph.getImageWidth()
              << " x " << pixelGraph.getImageHeight() << '\n';
    std::cout << "Node count: " << nodeCount << '\n';
    std::cout << "Edge count: " << edgeCount << '\n';
    std::cout << std::fixed << std::setprecision(2)
              << "Average outgoing degree: " << averageOutgoingDegree << '\n'
              << std::defaultfloat;
    std::cout << "Vertical connectivity sanity check: "
              << (pixelGraph.validateVerticalConnectivity() ? "passed" : "failed") << '\n';
}

void Application::computeShortestSeam()
{
    dijkstraSolver.solve(pixelGraph);
    printSeamDebugInfo();
    seamRenderer.updateFromSeam(dijkstraSolver.getSeam(), pixelGraph);
}

void Application::printSeamDebugInfo() const
{
    if (!dijkstraSolver.hasSeam())
    {
        std::cout << "Shortest seam: not computed (empty graph).\n";
        return;
    }

    const std::vector<unsigned int>& seamNodeIds = dijkstraSolver.getSeam();
    const unsigned int topNodeId = seamNodeIds.front();
    const unsigned int bottomNodeId = seamNodeIds.back();
    const auto [topX, topY] = pixelGraph.coordinatesFromNodeId(topNodeId);
    const auto [bottomX, bottomY] = pixelGraph.coordinatesFromNodeId(bottomNodeId);

    std::cout << "Shortest seam computed.\n";
    std::cout << "Seam length: " << seamNodeIds.size() << '\n';
    std::cout << std::fixed << std::setprecision(2)
              << "Total seam energy: " << dijkstraSolver.getTotalEnergy() << '\n'
              << std::defaultfloat;
    std::cout << "Top node: id=" << topNodeId
              << " (" << topX << ", " << topY << ")\n";
    std::cout << "Bottom node: id=" << bottomNodeId
              << " (" << bottomX << ", " << bottomY << ")\n";
    std::cout << "Seam validation: "
              << (dijkstraSolver.validateSeam(pixelGraph) ? "passed" : "failed") << '\n';
}

void Application::setLoadedStatus()
{
    std::ostringstream status;
    status << "Loaded: " << imageManager.getFilename()
           << " | " << imageManager.getWidth()
           << " x " << imageManager.getHeight();

    setStatus(status.str());
}

void Application::setStatus(const std::string& message)
{
    statusMessage = message;
    std::cout << statusMessage << '\n';
    updateWindowTitle();
}

void Application::updateWindowTitle()
{
    std::string title = WindowTitle;
    title += showingEnergyMap ? " | Energy Map" : " | Original Image";

    if (showingSeam)
    {
        title += " + Seam";
    }

    if (continuousCarvingEnabled)
    {
        title += " | Continuous Carving";
    }

    if (!statusMessage.empty())
    {
        title += " | " + statusMessage;
    }

    title += TitleHelpText;
    window.getRenderWindow().setTitle(title);
}
