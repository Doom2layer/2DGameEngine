# 2D Game Engine

A 2D game engine built with C++ and SDL2, featuring support for graphics rendering, scripting with Lua, and multimedia capabilities.

## Prerequisites

- Visual Studio 2019 or later with C++ development tools
- Git

## Getting Started

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd 2DGameEngine
   ```

2. **Build the project**:
   - Open `2DGameEngine.slnx` in Visual Studio
   - Select Debug or Release configuration
   - Press F5 to build and run

## Project Structure

- `/src/` - Source code (.cpp files)
- `/libs/` - Third-party libraries (GLM, ImGui, Lua, Sol2)
- `/assets/` - Game assets (images, fonts, sounds, scripts, tilemaps)
- `/Debug/` and `/x64/` - Build output directories

## Dependencies

The project uses the following external libraries (included in `/libs/`):
- **SDL2** - Graphics and input handling
- **GLM** - Mathematics library
- **ImGui** - UI framework
- **Lua/Sol2** - Scripting support

