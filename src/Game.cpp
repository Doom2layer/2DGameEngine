#include "Game.h"

#include <iostream>
#include <ostream>
#include <SDL.h>

Game::Game()
{
    bIsRunning = false;
    std::cout << "Game::Game()" << std::endl;
}

Game::~Game()
{
    std::cout << "Game::~Game()" << std::endl;   
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return;
    }
    
    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    WindowWidth = 800;//DisplayMode.w;
    WindowHeight = 600;//DisplayMode.h;
    
    Window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
    
    if (!Window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        return;   
    }
    
    Renderer = SDL_CreateRenderer(Window, -1, 0);
    
    if (!Renderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        return;   
    }
    SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
    bIsRunning = true;
}

void Game::Run()
{
    while (bIsRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    SDL_Event SDLEvent;
    while (SDL_PollEvent(&SDLEvent))
    {
        switch (SDLEvent.type)
        {
            case SDL_QUIT:
                bIsRunning = false;
            break;
            
            case SDL_KEYDOWN:
            if (SDLEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                bIsRunning = false;
            }
            break;
        }
    }
}

void Game::Update()
{
    //Todo: Update Game Objects...
}

void Game::Render()
{
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    SDL_RenderClear(Renderer);
    //Todo: Render All Game Objects...
    SDL_RenderPresent(Renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}
