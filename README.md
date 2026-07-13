# SeamCraft

SeamCraft is an interactive desktop application for learning content-aware image resizing with Seam Carving and Dijkstra's Algorithm.

The project is currently at Milestone 2B and contains user-controlled image loading infrastructure needed for future seam carving work.

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

Planned:

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

Milestone 2B: User-controlled image loading.

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
|   |-- ImageManager.hpp
|   `-- Window.hpp
|-- src/
|   |-- Application.cpp
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
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/Window.cpp src/ImageManager.cpp build/tinyfiledialogs.o -Iinclude -Ithird_party/tinyfiledialogs -lsfml-graphics -lsfml-window -lsfml-system -lcomdlg32 -lole32 -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` to build and `F5` to debug.

See `BUILD.md` for full setup instructions.

## Roadmap

See `PROJECT_PLAN.md` for the full milestone roadmap.

## Future Work

- Energy calculation
- Graph construction
- Dijkstra seam search
- Seam carving
- Animation
- Saving resized images
