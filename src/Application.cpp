#include "Application.hpp"

#include "tinyfiledialogs.h"

#include <algorithm>
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
constexpr const char TitleHelpText[] = " | O: Open Image | R: Reset | E: Toggle Energy";
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
        if (showingEnergyMap)
        {
            energyRenderer.fitToWindow(window.getRenderWindow().getSize());
        }
        else
        {
            imageManager.fitToWindow(window.getRenderWindow().getSize());
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
}

void Application::openImage()
{
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

    if (!statusMessage.empty())
    {
        title += " | " + statusMessage;
    }

    title += TitleHelpText;
    window.getRenderWindow().setTitle(title);
}
