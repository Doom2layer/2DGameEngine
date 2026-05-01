#pragma once

#include <SDL.h>

class Game
{
public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    
    int WindowWidth;
    int WindowHeight;
    
private:
    
    bool bIsRunning;
    
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    
};

