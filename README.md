# 2D Game Engine

A 2D game engine built with C++ and SDL2, featuring support for graphics rendering, scripting with Lua, and multimedia capabilities.

## Project Setup

### Prerequisites
- Visual Studio 2019 or later with C++ development tools
- Git

### Initial Setup

1. **Clone the repository** (if from remote):
   ```bash
   git clone <repository-url>
   cd 2DGameEngine
   ```

2. **Update your local git configuration** (set your name and email):
   ```bash
   git config user.name "Your Name"
   git config user.email "your.email@example.com"
   ```

3. **Build the project**:
   - Open `2DGameEngine.slnx` in Visual Studio
   - Select Debug or Release configuration
   - Press F5 to build and run

### Project Structure

- `/src/` - Source code (.cpp files)
- `/libs/` - Third-party libraries (GLM, ImGui, Lua, Sol2)
- `/assets/` - Game assets (images, fonts, sounds, scripts, tilemaps)
- `/Debug/` and `/x64/` - Build output directories (not tracked)

### Dependencies

The project uses the following external libraries (included in `/libs/`):
- **SDL2** - Graphics and input handling
- **GLM** - Mathematics library
- **ImGui** - UI framework
- **Lua/Sol2** - Scripting support

## Git Workflow

### Before Your First Commit

Update the user configuration with your actual information:
```bash
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### Making Commits

1. **Stage changes**:
   ```bash
   git add .
   ```

2. **Commit with a descriptive message**:
   ```bash
   git commit -m "Description of what changed"
   ```

3. **Push to remote** (if available):
   ```bash
   git push origin main
   ```

### What Gets Ignored

The `.gitignore` file automatically excludes:
- Build artifacts (`.obj`, `.ilk`, `.pdb`, `.exe`)
- IDE files (`.vs/`, `*.user`)
- Build directories (`Debug/`, `Release/`, `x64/`)
- Visual Studio temporary files
- DLL files (except development headers)

This keeps the repository clean and only tracks source code, project files, and assets.

## Development Tips

1. Always pull before starting work: `git pull origin main`
2. Create feature branches for new features: `git checkout -b feature/my-feature`
3. Keep commits atomic and well-documented
4. Run a clean build periodically to catch any issues

## Troubleshooting

If you get `.tlog` or object files in git:
1. Run: `git rm -r --cached .` (this removes all cached files)
2. Verify `.gitignore` has the correct patterns
3. Commit again: `git add . && git commit -m "Remove build artifacts"`

## License

[Add your license information here]

