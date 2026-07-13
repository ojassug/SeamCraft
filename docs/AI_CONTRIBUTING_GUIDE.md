# SeamCraft AI Contributing Guide

This document is the permanent guide for all AI coding assistants (Gemini, GPT, Claude, Codex, etc.) working on the SeamCraft project. It defines the development philosophy, project architecture, and coding standards. Follow this guide strictly to ensure continuity and maintain project integrity.

---

## Project Overview

SeamCraft is an interactive desktop application designed to demonstrate content-aware image resizing using the Seam Carving algorithm. 

* **Educational Purpose:** The codebase serves as an educational tool for computer science students. The implementation must remain clean, highly readable, and free of overly complex template programming or excessive abstractions.
* **Overall Goal:** To allow users to load images, visualize pixel energy maps, compute the lowest-energy seams using Dijkstra's algorithm, see the seams overlaid on the image, and ultimately remove these seams to resize the image without distorting key features.

---

## Technology Stack

All development must target the following stack:
* **Language:** Modern C++17
* **Graphics Library:** SFML (Simple and Fast Multimedia Library) 3.x
* **Toolchain:** MSYS2 UCRT64 (GCC/G++)
* **Operating System:** Windows 11

---

## Architecture

SeamCraft utilizes a modular, component-based architecture where class responsibilities are strictly decoupled. The high-level orchestration is handled solely by the `Application` class.

```text
Application
├── ImageManager
├── EnergyCalculator
├── EnergyRenderer
├── PixelGraph
├── DijkstraSolver
└── SeamRenderer
```

### Separation of Concerns
* The **Application** class orchestrates the lifecycle and state transitions. It acts as the "glue" but contains no algorithmic logic.
* **Component classes** are self-contained. They accept input data, perform their designated tasks, and expose their results. They do not talk to each other directly; orchestration goes through the `Application` class.

---

## Responsibilities of Every Class

### 1. ImageManager
* **Responsible For:** Loading images from disk, caching the original and current working images, fitting/scaling the image to the window dimensions while preserving aspect ratio, and rendering the active image.
* **Must NEVER Be Responsible For:** Calculating pixel energy, building graphs, running shortest-path algorithms, or managing seam state.

### 2. EnergyCalculator
* **Responsible For:** Computing pixel energy using grayscale luminance and Sobel gradients, and storing the energy values in a 2D float map.
* **Must NEVER Be Responsible For:** Loading/saving images, rendering graphics, or pathfinding.

### 3. EnergyRenderer
* **Responsible For:** Normalizing the energy map (min-max normalization) to a grayscale texture, fitting the visualization to the window size, and drawing it.
* **Must NEVER Be Responsible For:** Calculating the energy values, managing the source image lifecycle, or computing seams.

### 4. PixelGraph
* **Responsible For:** Constructing a directed graph from the energy map where pixels map to nodes using deterministic coordinate formulas (`nodeId = y * imageWidth + x`), and establishing vertical connectivity (down-left, down, down-right neighbors).
* **Must NEVER Be Responsible For:** Calculating energy, rendering nodes/edges, or solving shortest paths.

### 5. DijkstraSolver
* **Responsible For:** Running Dijkstra's algorithm on the `PixelGraph` to find the path of minimum total energy from the top row to the bottom row, and reconstructing/validating the resulting seam.
* **Must NEVER Be Responsible For:** Modifying the image, constructing the graph structure, or drawing the seam.

### 6. SeamRenderer
* **Responsible For:** Drawing a visual representation of the seam on top of the original image. It maps the seam's node IDs back to pixel coordinates and displays them in a bright contrasting color (e.g., Red).
* **Must NEVER Be Responsible For:** Running Dijkstra's algorithm, modifying the underlying image pixels, or building the graph.

---

## Coding Style

To keep the project accessible to second-year computer science students:
* **Readability over Cleverness:** Avoid templates, inheritance, and generic programming unless strictly required. Write explicit, straightforward code.
* **Meaningful Naming:** Variables, classes, and functions must have descriptive, self-explanatory names.
* **Short Functions:** Break logic down into small, single-purpose functions.
* **Clear Algorithm Comments:** Document complex mathematical or algorithmic steps (like Dijkstra relaxation or Sobel operations) with inline explanations.
* **No Unnecessary Abstractions:** Do not create interfaces or abstract base classes. Concrete classes are preferred.
* **Zero Compiler Warnings:** Always build with the flags `-Wall -Wextra -pedantic`. Treat warnings as errors and fix them immediately.

---

## Development Rules

* **Inspect First:** Before writing any code, examine the current directory structure and read the existing source files to match code patterns, naming conventions, and layout.
* **Preserve the Architecture:** Never change the core architecture. Do not rename classes, merge classes, or move responsibilities between them.
* **Extend, Don't Rewrite:** Build upon the existing classes. Write new methods or introduce new classes only to fulfill the exact milestone requested.

---

## Milestone Rules

* **Scope Control:** Implement **ONLY** the requested milestone.
* **No Future Work:** Do not implement features or functions reserved for future milestones. Do not write placeholder code for future steps.
* **Milestone Independence:** Ensure the project is fully functional, compiling, and running at the end of the current milestone before marking it complete.

---

## Documentation Rules

* **Targeted Edits:** Update documentation (such as `README.md` or `PROJECT_PLAN.md`) only when the implemented milestone introduces new controls, features, or architectural classes.
* **Keep it Clean:** Avoid irrelevant changes or stylistic reformatting of parts of the documentation that were not affected by your work.

---

## Build & Testing Rules

* **Compilation:** Verify that the project compiles cleanly under MSYS2 UCRT64 with the specified compiler flags.
* **Regression Testing:** Verify that previous features (loading images, resetting, toggling energy maps, etc.) still work perfectly.
* **Smoke Testing:** When testing new algorithms, create temporary unit tests or small synthetic datasets to verify logic correctness (e.g., testing the solver on a tiny mock graph).
* **Clean Up:** Delete all temporary test files, mock assets, and test main files before concluding the task.

---

## Expected Final Response

At the conclusion of your milestone implementation, always provide a summary with the following structure:
1. **Files Added:** A list of all newly created files and their purposes.
2. **Files Modified:** A list of modified files and a summary of the edits.
3. **Architecture Explanation:** A brief description of how the new implementation fits into the existing architecture.
4. **Testing Performed:** The steps taken to test the new functionality (including compiler warnings checked, manual UI checks, and smoke tests run/removed).
5. **Suggested Git Commit Message:** A concise, imperative-style git commit message describing the changes.

---

## Project Philosophy

SeamCraft prioritizes:
1. **Clean Architecture:** Keeping components strictly isolated.
2. **Educational Clarity:** Writing readable C++ that serves as a learning reference.
3. **Maintainability:** Structuring files and APIs so that consecutive developers (human or AI) can pick up the project seamlessly.
4. **Modular Design:** Designing single-responsibility modules that do one thing and do it well.
