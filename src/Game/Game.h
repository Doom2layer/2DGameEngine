#pragma once

#include <SDL.h>
#include <memory>

#include "../AssetManager/AssetManager.h"

class ECSManager;
constexpr int FRAMES_PER_SECOND = 60;
constexpr int MILLISECONDS_PER_FRAME = 1000 / FRAMES_PER_SECOND;

class Game
{
public:
    Game();
    ~Game();
    void Initialize();
    void Setup();
    void LoadLevel(int LevelNumber);
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    
    // 4-byte types
    int WindowWidth;
    int WindowHeight;
    
private:
    // 8-byte types
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    
    std::unique_ptr<ECSManager> ECSManagerInstance;
    std::unique_ptr<AssetManager> AssetManagerInstance;
    
    // 4-byte types
    int MilliSecondsPreviousFrame{0};
    
    // 1-byte types
    bool bIsRunning;

    
};

/*
 * Memory Layout - Alignment Reference
 * ─────────────────────────────────────────────────────
 * Rule: Declare members largest to smallest to avoid padding waste.
 *
 * Alignment Requirements:
 *   8 bytes  →  pointers (64-bit), double, int64_t, uint64_t
 *   4 bytes  →  int, uint32_t, float, enum (default)
 *   2 bytes  →  short, int16_t, uint16_t
 *   1 byte   →  bool, char, int8_t, uint8_t
 *
 * Example (bad - 7 bytes wasted):
 *   bool     bFlag;        // offset 0  [1 byte]
 *   [3 bytes padding]      // offset 1  compiler inserted
 *   int      Value;        // offset 4  [4 bytes]
 *   [4 bytes padding]      // offset 8  compiler inserted
 *   double   Data;         // offset 16 [8 bytes]  ← needs 8-byte alignment
 *
 * Example (good - 0 bytes wasted):
 *   double   Data;         // offset 0  [8 bytes]
 *   int      Value;        // offset 8  [4 bytes]
 *   bool     bFlag;        // offset 12 [1 byte]
 *   [3 bytes trailing pad] // offset 13 harmless, required for array alignment
 */