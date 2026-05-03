#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"


#include <SDL.h>
#include <glm/glm.hpp>


Game::Game()
{
    bIsRunning = false;
    Manager = std::make_unique<ECSManager>();
    Logger::Log("Game::Game() Called Game Object Constructor Called");
}

Game::~Game()
{
    Logger::Log("Game::~Game() Called Game Object Destructor Called");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Error("SDL_Init failed: " + std::string(SDL_GetError()));
        return;
    }
    
    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    WindowWidth = 800;//DisplayMode.w;
    WindowHeight = 600;//DisplayMode.h;
    
    Window = SDL_CreateWindow("Mustafa's 2D Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
    
    if (!Window)
    {
        Logger::Error("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
        return;   
    }
    
    Renderer = SDL_CreateRenderer(Window, -1, 0);
    
    if (!Renderer)
    {
        Logger::Error("SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
        return;   
    }
    /*
    SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
    */
    bIsRunning = true;
}

void Game::Setup()
{
    //Todo: Initialize game objects ...
    //Create Entity ECSManager.CreateEntity();    
    Manager->AddSystem<MovementSystem>();
    Manager->AddSystem<RenderSystem>();
    
    //Create an entity and Add Some Components to the entity
    Entity Tank = Manager->CreateEntity();
    Tank.AddComponent<FTransformComponent>(glm::vec2(10.0f, 30.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Tank.AddComponent<FRigidBodyComponent>(glm::vec2(40.0f, 0.0f));
    Tank.AddComponent<FSpriteComponent>(10, 10);
    
    Entity Truck = Manager->CreateEntity();
    Truck.AddComponent<FTransformComponent>(glm::vec2(50.0f, 100.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Truck.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 50.0f));
    Truck.AddComponent<FSpriteComponent>(10, 50);
}

void Game::Run()
{
    Setup();
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
    // if we are running too fast, we waste some time until we reach the MILLISECONDS_PER_FRAME
    int TimeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - MilliSecondsPreviousFrame);
    
    if (TimeToWait > 0 && TimeToWait <= MILLISECONDS_PER_FRAME)
    {
        SDL_Delay(TimeToWait);
    }
    
    // The difference in ticks since last frame, converted into seconds
    double DeltaTime = (SDL_GetTicks() - MilliSecondsPreviousFrame) / 1000.0;
    
    // Store the current frame time
    MilliSecondsPreviousFrame = SDL_GetTicks();
    
    // Update the manager to process the entities that are waiting to be created/deleted
    Manager->Update();
    
    // Ask all the systems to update
    Manager->GetSystem<MovementSystem>().Update(DeltaTime);
}

void Game::Render()
{
    SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
    SDL_RenderClear(Renderer);
    
    // Ask all the systems that need to render
    Manager->GetSystem<RenderSystem>().Update(Renderer);

    SDL_RenderPresent(Renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}
