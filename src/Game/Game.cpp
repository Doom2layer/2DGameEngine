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
#include <fstream>


Game::Game()
{
    bIsRunning = false;
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
        Logger::Error("Game::Initialize failed to create renderer.");
        return;
    }
    
    ECSManagerInstance = std::make_unique<ECSManager>();
    AssetManagerInstance = std::make_unique<AssetManager>(Renderer);

    
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

void Game::LoadLevel(int LevelNumber)
{
    // Add the system that need to be processed in the game
    ECSManagerInstance->AddSystem<MovementSystem>();
    ECSManagerInstance->AddSystem<RenderSystem>();
    
    // Adding assets to the asset manager
    AssetManagerInstance->AddTexture("Tank-Image", "./assets/images/tank-panther-right.png");
    AssetManagerInstance->AddTexture("Truck-Image", "./assets/images/truck-ford-right.png");
    AssetManagerInstance->AddTexture("Jungle-Tilemap-Image", "./assets/tilemaps/jungle.png");
    
    //Load the tile map
    constexpr int TileSize =32;
    constexpr double TileScale = 1;
    constexpr int MapNumberColumns = 25;
    constexpr int MapNumberRows = 20;
    std::fstream MapFile;
    MapFile.open("./assets/tilemaps/jungle.map");
    if (!MapFile.is_open())
    {
        Logger::Error("Failed to open map file.");
        return;
    }
    
    for (int y = 0; y < MapNumberRows; y++)
    {
        for (int x = 0; x < MapNumberColumns; x++)
        {
            char TileType;
            MapFile.get(TileType);
            int SourceRectY = (TileType - '0') * TileSize;
            MapFile.get(TileType);
            int SourceRectX = (TileType - '0') * TileSize;
            MapFile.ignore();
            
            Entity Tile = ECSManagerInstance->CreateEntity();
            Tile.AddComponent<FTransformComponent>(glm::vec2(x * TileSize * TileScale, y * TileSize * TileScale), glm::vec2(TileScale, TileScale), 0.0f);
            Tile.AddComponent<FSpriteComponent>("Jungle-Tilemap-Image", TileSize, TileSize, SourceRectX, SourceRectY, ERenderLayer::Background, 0);
        }
    }
    MapFile.close();    
    
    //Create an entity and Add Some Components to the entity
    Entity Tank = ECSManagerInstance->CreateEntity();
    Tank.AddComponent<FTransformComponent>(glm::vec2(10.0f, 30.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Tank.AddComponent<FRigidBodyComponent>(glm::vec2(40.0f, 0.0f));
    Tank.AddComponent<FSpriteComponent>("Tank-Image", 32, 32, 0, 0, ERenderLayer::Enemy, 0);
    
    Entity Truck = ECSManagerInstance->CreateEntity();
    Truck.AddComponent<FTransformComponent>(glm::vec2(10.0f, 30.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Truck.AddComponent<FRigidBodyComponent>(glm::vec2(40.0f, 0.0f));
    Truck.AddComponent<FSpriteComponent>("Truck-Image", 32, 32, 0, 0, ERenderLayer::Player, 0);

}

void Game::Setup()
{
    LoadLevel(0);
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
    ECSManagerInstance->Update();
    
    // Ask all the systems to update
    ECSManagerInstance->GetSystem<MovementSystem>().Update(DeltaTime);
}

void Game::Render()
{
    SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
    SDL_RenderClear(Renderer);
    
    // Ask all the systems that need to render
    ECSManagerInstance->GetSystem<RenderSystem>().Update(Renderer, AssetManagerInstance);

    SDL_RenderPresent(Renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}
