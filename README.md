# SeamCraft

SeamCraft is an interactive desktop application for learning content-aware image resizing with Seam Carving and Dijkstra's Algorithm.

The project is currently at Milestone 6 and can remove vertical seams dynamically using Seam Carving.

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
- Displays the energy map as a grayscale visualization
- Toggles between the original image and energy map with the `E` key
- Regenerates the energy visualization after image load or reset
- Builds a vertical-seam pixel graph after energy recalculation
- Computes the minimum-energy vertical seam using Dijkstra's algorithm
- Validates seam structure automatically
- Prints temporary seam, graph, and energy debug statistics to the console
- Converts Dijkstra node IDs back to pixel coordinates
- Draws the computed seam as a bright red overlay on top of the original image
- Toggles seam visualization with the `S` key
- Removes a single vertical seam dynamically with the `C` key, shrinking the image width by one pixel
- Automatically updates the energy map, graph, shortest seam, and seam overlay on removal
- Toggles continuous seam carving with the `Space` key, removing seams at a fixed interval while the window remains responsive

Planned:

- Seam removal animation

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

Milestone 6: Vertical seam removal.
## Energy Calculation

Seam carving removes low-importance paths through an image. Before SeamCraft can find those paths, it needs an energy map. The energy map stores one `float` value per pixel, where larger values usually mean stronger visual detail such as edges or texture.

`EnergyCalculator` computes this map from the current `sf::Image`. It does not draw anything and does not modify the image. `ImageManager` still owns image loading, reset, and display state.

The current energy model uses the Sobel operator:

- Each neighboring pixel is converted to grayscale with `Gray = 0.299R + 0.587G + 0.114B`.
- A horizontal Sobel kernel estimates left-to-right intensity change.
- A vertical Sobel kernel estimates top-to-bottom intensity change.
- The final energy is `sqrt(horizontalGradient^2 + verticalGradient^2)`.

Border pixels use replicated-border handling. If the Sobel kernel asks for a neighbor outside the image, SeamCraft reuses the nearest valid edge pixel. This keeps every pixel, including corners, simple to calculate.

## Energy Visualization

`EnergyRenderer` converts the floating-point energy map into a grayscale `sf::Image`, then stores it in an `sf::Texture` and `sf::Sprite` for drawing. It fits the visualization inside the application window, preserves the image aspect ratio, and centers it.

The renderer uses min-max normalization:

- Find the minimum energy value in the map.
- Find the maximum energy value in the map.
- Convert each energy value into the range `0` to `255`.
- The minimum energy becomes black.
- The maximum energy becomes white.
- Intermediate values become gray.

If all energy values are the same, the renderer displays them as black because there is no range to stretch.

## Pixel Graph

`PixelGraph` converts the energy map into a graph that a later Dijkstra milestone can use. Each pixel becomes exactly one `GraphNode`.

Each node stores:

- unique node id
- x coordinate
- y coordinate
- energy value

Node ids are deterministic:

```text
nodeId = y * imageWidth + x
```

This makes it simple to move between a pixel coordinate and a node id. `coordinatesFromNodeId(nodeId)` reverses it with division and modulo.

The graph is built only for vertical seam carving. Each pixel connects to valid pixels directly below it:

- down-left: `(x - 1, y + 1)`
- down: `(x, y + 1)`
- down-right: `(x + 1, y + 1)`

Edges outside the image are skipped. Because seams move from top to bottom, bottom-row pixels have no outgoing edges. Corner pixels have fewer neighbours than interior pixels.

The edge weight is the energy value of the destination pixel. This means a future shortest-path algorithm will pay the cost of entering the next pixel in the seam.

## Dijkstra Shortest Path

`DijkstraSolver` runs Dijkstra's algorithm on the `PixelGraph` to find the single minimum-energy vertical seam.

The algorithm:

1. Every top-row pixel is seeded into a min-heap priority queue with its own energy as the initial distance.
2. Nodes are settled in order of increasing distance. For each settled node, every outgoing edge is relaxed: if `distance[u] + edgeWeight < distance[v]`, the shorter path is recorded.
3. The priority queue uses `std::priority_queue` with `std::greater` to get smallest-distance-first ordering. Duplicate entries (from multiple relaxations) are handled with lazy deletion — already-visited nodes are skipped.
4. After the queue is empty, the bottom-row pixel with the smallest total distance is selected.
5. The seam is reconstructed by walking the predecessor array from the best bottom-row pixel back to the top row.

The seam is returned as `std::vector<unsigned int>` containing node ids ordered from the top row (index 0) to the bottom row (index height-1). Node ids were chosen over `GraphNode` objects because the existing `PixelGraph` API is node-id-based, making this representation the most natural for integration.

Automatic validation checks:

- Seam length equals image height
- First node is in the top row
- Last node is in the bottom row
- Consecutive nodes differ by at most one column

## Seam Visualization

`SeamRenderer` visualizes the seam computed by `DijkstraSolver`. It draws a bright red overlay on top of the displayed image.

- It takes the shortest-path node IDs and the `PixelGraph`.
- It converts each node ID back into an `(x, y)` pixel coordinate using `coordinatesFromNodeId`.
- It creates a transparent image the same size as the original image and sets the seam pixels to red.
- It uses the same scaling and positioning logic (`fitToWindow`) as the other renderers, ensuring the overlay perfectly aligns with the displayed image.
- It does not modify the original image data and does not recompute the seam.

## Seam Removal

`SeamRemover` handles removing the computed seam from an `sf::Image` to reduce the image's width.

- It takes the active image, the seam node IDs, and the `PixelGraph`.
- It creates a new image that is exactly one pixel narrower.
- For each row, it copies all pixels to the left of the seam as-is, and shifts all pixels to the right of the seam left by one pixel.
- It contains no rendering, graph-building, or pathfinding logic.

## Controls

- Press `O` to open an image from disk
- Press `R` to reset the current image back to the stored original image
- Press `E` to toggle between the original image and the energy map
- Press `S` to toggle the seam overlay on the original image
- Press `C` to carve (remove) the active shortest vertical seam from the image
- Press `Space` to start or stop continuous seam carving

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
|   |-- DijkstraSolver.hpp
|   |-- EnergyCalculator.hpp
|   |-- EnergyRenderer.hpp
|   |-- ImageManager.hpp
|   |-- PixelGraph.hpp
|   |-- SeamRenderer.hpp
|   |-- SeamRemover.hpp
|   `-- Window.hpp
|-- src/
|   |-- Application.cpp
|   |-- DijkstraSolver.cpp
|   |-- EnergyCalculator.cpp
|   |-- EnergyRenderer.cpp
|   |-- ImageManager.cpp
|   |-- main.cpp
|   |-- PixelGraph.cpp
|   |-- SeamRenderer.cpp
|   |-- SeamRemover.cpp
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
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/DijkstraSolver.cpp src/Window.cpp src/ImageManager.cpp src/EnergyCalculator.cpp src/EnergyRenderer.cpp src/PixelGraph.cpp src/SeamRenderer.cpp src/SeamRemover.cpp build/tinyfiledialogs.o -Iinclude -Ithird_party/tinyfiledialogs -lsfml-graphics -lsfml-window -lsfml-system -lcomdlg32 -lole32 -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` to build and `F5` to debug.

See `BUILD.md` for full setup instructions.

## Roadmap

See `PROJECT_PLAN.md` for the full milestone roadmap.

## Future Work

- Seam removal
- Seam carving animation
- Saving resized images
