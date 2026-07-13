# SeamCraft

SeamCraft is an interactive desktop application for learning content-aware image resizing with Seam Carving and Dijkstra's Algorithm.

The project is currently at Milestone 3A and contains the first seam-carving computation step: energy map generation.

## Features

Current:

- Opens an SFML window titled `SeamCraft`
- Uses a fixed 1200x800 window size
- Runs at 60 FPS
- Handles window close events cleanly
- Uses separate `Application` and `Window` classes
- Attempts to load `assets/images/sample.png` at startup
- Lets the user open an image from disk with the `O` key
- Supports PNG, JPG, and JPEG user-selected images
- Lets the user reset the current image back to the stored original image with the `R` key
- Displays a loaded image centered in the window
- Preserves image aspect ratio
- Scales large images down without upscaling small images
- Stores both the original image and the current working image
- Shows simple status information in the window title
- Computes a floating-point energy value for every pixel after image load or reset
- Keeps the energy map ready internally for future seam carving milestones
- Prints temporary energy debug statistics to the console

Planned:

- Pixel graph construction
- Dijkstra shortest path seam search
- Seam highlighting and removal
- Saving resized images

## Tech Stack

- C++17
- SFML 3.0.2
- GCC 16.1.0
- MSYS2 UCRT64
- VS Code
- Windows 11
- tinyfiledialogs, vendored in `third_party/tinyfiledialogs`

## Prerequisites

- Windows 11
- MSYS2 UCRT64
- GCC 16.1.0
- SFML 3.0.2
- VS Code
- Git

Install GCC, GDB, and SFML from the `MSYS2 UCRT64` terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-sfml
```

## Current Milestone

Milestone 3A: Energy calculation.

## Energy Calculation

Seam carving removes low-importance paths through an image. Before SeamCraft can find those paths, it needs an energy map. The energy map stores one `float` value per pixel, where larger values usually mean stronger visual detail such as edges or texture.

`EnergyCalculator` computes this map from the current `sf::Image`. It does not draw anything and does not modify the image. `ImageManager` still owns image loading, reset, and display state.

The current energy model uses the Sobel operator:

- Each neighboring pixel is converted to grayscale with `Gray = 0.299R + 0.587G + 0.114B`.
- A horizontal Sobel kernel estimates left-to-right intensity change.
- A vertical Sobel kernel estimates top-to-bottom intensity change.
- The final energy is `sqrt(horizontalGradient^2 + verticalGradient^2)`.

Border pixels use replicated-border handling. If the Sobel kernel asks for a neighbor outside the image, SeamCraft reuses the nearest valid edge pixel. This keeps every pixel, including corners, simple to calculate.

## Controls

- Press `O` to open an image from disk
- Press `R` to reset the current image back to the stored original image

## Supported Image Formats

- PNG
- JPG
- JPEG

## Folder Structure

```text
SeamCraft/
|-- assets/
|   |-- icons/
|   `-- images/
|-- docs/
|-- include/
|   |-- Application.hpp
|   |-- EnergyCalculator.hpp
|   |-- ImageManager.hpp
|   `-- Window.hpp
|-- src/
|   |-- Application.cpp
|   |-- EnergyCalculator.cpp
|   |-- ImageManager.cpp
|   |-- main.cpp
|   `-- Window.cpp
|-- third_party/
|   `-- tinyfiledialogs/
|       |-- tinyfiledialogs.c
|       `-- tinyfiledialogs.h
|-- .gitignore
|-- BUILD.md
|-- CONTRIBUTING.md
|-- LICENSE
|-- PROJECT_PLAN.md
`-- README.md
```

## How to Build

From the project root:

```bash
mkdir -p build
gcc -std=c99 -g -c third_party/tinyfiledialogs/tinyfiledialogs.c -o build/tinyfiledialogs.o
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/Window.cpp src/ImageManager.cpp src/EnergyCalculator.cpp build/tinyfiledialogs.o -Iinclude -Ithird_party/tinyfiledialogs -lsfml-graphics -lsfml-window -lsfml-system -lcomdlg32 -lole32 -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` to build and `F5` to debug.

See `BUILD.md` for full setup instructions.

## Roadmap

See `PROJECT_PLAN.md` for the full milestone roadmap.

## Future Work

- Graph construction
- Dijkstra seam search
- Seam carving
- Animation
- Saving resized images
