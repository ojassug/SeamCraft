# Build Guide

This guide is for Windows 11 with MSYS2 UCRT64, GCC, SFML, and VS Code.

## Install MSYS2

Download and install MSYS2 from:

```text
https://www.msys2.org/
```

Use the `MSYS2 UCRT64` terminal for the commands below.

## Update Pacman

```bash
pacman -Syu
```

If MSYS2 asks you to close the terminal, close it, reopen `MSYS2 UCRT64`, then run:

```bash
pacman -Su
```

## Install GCC and GDB

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb
```

## Install SFML

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-sfml
```

## File Dialog Dependency

SeamCraft vendors `tinyfiledialogs` in `third_party/tinyfiledialogs`.
It is a small cross-platform C library used only for the Open Image file picker.

## Build

From the project root:

```bash
mkdir -p build
gcc -std=c99 -g -c third_party/tinyfiledialogs/tinyfiledialogs.c -o build/tinyfiledialogs.o
g++ -std=c++17 -Wall -Wextra -pedantic -g src/main.cpp src/Application.cpp src/Window.cpp src/ImageManager.cpp build/tinyfiledialogs.o -Iinclude -Ithird_party/tinyfiledialogs -lsfml-graphics -lsfml-window -lsfml-system -lcomdlg32 -lole32 -o build/SeamCraft.exe
```

In VS Code, press `Ctrl+Shift+B` and choose `Build SeamCraft`.

## Run

From the project root:

```bash
./build/SeamCraft.exe
```

If running from PowerShell, make sure `C:\msys64\ucrt64\bin` is in your `PATH` so Windows can find the SFML DLLs.

## Debug

Open the project in VS Code and press `F5`.

The debugger configuration uses:

```text
C:/msys64/ucrt64/bin/gdb.exe
```
