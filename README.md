# SeamCraft

SeamCraft is an interactive desktop application for learning content-aware image resizing with Seam Carving and Dijkstra's Algorithm.

The project is currently at Milestone 2A and contains the first image infrastructure needed for future seam carving work.

## Features

Current:

- Opens an SFML window titled `SeamCraft`
- Uses a fixed 1200x800 window size
- Runs at 60 FPS
- Handles window close events cleanly
- Uses separate `Application` and `Window` classes
- Attempts to load `assets/images/sample.png` at startup
- Displays a loaded image centered in the window
- Preserves image aspect ratio
- Scales large images down without upscaling small images
- Stores both the original image and the current working image

Planned:

- User-selected image loading
- Energy map generation
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

Milestone 2A: Image infrastructure.

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
|   |-- ImageManager.hpp
|   `-- Window.hpp
|-- src/
|   |-- Application.cpp
|   |-- ImageManager.cpp
|   |-- main.cpp
|   `-- Window.cpp
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
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/Window.cpp src/ImageManager.cpp -Iinclude -lsfml-graphics -lsfml-window -lsfml-system -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` to build and `F5` to debug.

See `BUILD.md` for full setup instructions.

## Roadmap

See `PROJECT_PLAN.md` for the full milestone roadmap.

## Future Work

- User-selected image loading
- Energy calculation
- Graph construction
- Dijkstra seam search
- Seam carving
- Animation
- Saving resized images
