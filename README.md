# SeamCraft

SeamCraft is an interactive desktop application for learning content-aware image resizing with Seam Carving and Dijkstra's Algorithm.

The project is currently at Milestone 1 and contains a clean C++/SFML application foundation.

## Features

Current:

- Opens an SFML window titled `SeamCraft`
- Uses a fixed 1200x800 window size
- Runs at 60 FPS
- Handles window close events cleanly
- Uses separate `Application` and `Window` classes

Planned:

- Image loading and rendering
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

Milestone 1: Project foundation.

## Folder Structure

```text
SeamCraft/
|-- assets/
|   |-- icons/
|   `-- images/
|-- docs/
|-- include/
|   |-- Application.hpp
|   `-- Window.hpp
|-- src/
|   |-- Application.cpp
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
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/Window.cpp -Iinclude -lsfml-graphics -lsfml-window -lsfml-system -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` to build and `F5` to debug.

See `BUILD.md` for full setup instructions.

## Roadmap

See `PROJECT_PLAN.md` for the full milestone roadmap.

## Future Work

- Image loading
- Energy calculation
- Graph construction
- Dijkstra seam search
- Seam carving
- Animation
- Saving resized images
