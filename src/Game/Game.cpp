#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/2DBoxColliderComponent.h"
#include "../Components/2DCircleColliderComponent.h"
#include "../Components/PlayerControllerComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/CollisionSystem2D.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/RenderColliderSystem2D.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifeCycleSystem.h"

#include <SDL.h>
#include <glm/glm.hpp>
#include <fstream>


int Game::WindowWidth;
int Game::WindowHeight;
int Game::MapWidth;
int Game::MapHeight;


Game::Game()
{
    bIsRunning = false;
    bIsDebug = false;
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
    
    // Initialize the camera view with the entire screen area
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = WindowWidth;
    Camera.h = WindowHeight;
    
    ECSManagerInstance = std::make_unique<ECSManager>();
    AssetManagerInstance = std::make_unique<AssetManager>(Renderer);
    EventManagerInstance = std::make_unique<EventManager>();

    
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
    ECSManagerInstance->AddSystem<AnimationSystem>();
    ECSManagerInstance->AddSystem<MovementSystem>();
    ECSManagerInstance->AddSystem<CameraMovementSystem>();   
    ECSManagerInstance->AddSystem<RenderSystem>();
    ECSManagerInstance->AddSystem<CollisionSystem2D>();
    ECSManagerInstance->AddSystem<RenderColliderSystem2D>();
    ECSManagerInstance->AddSystem<DamageSystem>();
    ECSManagerInstance->AddSystem<KeyboardControlSystem>();
    ECSManagerInstance->AddSystem<ProjectileEmitSystem>();
    ECSManagerInstance->AddSystem<ProjectileLifeCycleSystem>();
    
    // Adding assets to the asset manager
    AssetManagerInstance->AddTexture("Chopper-Image", "./assets/images/chopper-spritesheet.png");
    AssetManagerInstance->AddTexture("Tank-Image", "./assets/images/tank-panther-right.png");
    AssetManagerInstance->AddTexture("Truck-Image", "./assets/images/truck-ford-right.png");
    AssetManagerInstance->AddTexture("Jungle-Tilemap-Image", "./assets/tilemaps/jungle.png");
    AssetManagerInstance->AddTexture("Radar-Image", "./assets/images/radar.png");
    AssetManagerInstance->AddTexture("Bullet-Image", "./assets/images/bullet.png");
    
    
    //Load the tile map
    constexpr int TileSize =32;
    constexpr double TileScale = 2;
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
            Tile.Group("Tiles");
            Tile.AddComponent<FTransformComponent>(glm::vec2(x * TileSize * TileScale, y * TileSize * TileScale), glm::vec2(TileScale, TileScale), 0.0f);
            Tile.AddComponent<FSpriteComponent>("Jungle-Tilemap-Image", TileSize, TileSize, SourceRectX, SourceRectY, ERenderLayer::Background, 0, false);
        }
    }
    MapFile.close();    
    MapWidth = MapNumberColumns * TileSize * TileScale;
    MapHeight = MapNumberRows * TileSize * TileScale;
    
    //Create an entity and Add Some Components to the entity
    Entity Chopper = ECSManagerInstance->CreateEntity();
    Chopper.Tag("Player");
    Chopper.AddComponent<FTransformComponent>(glm::vec2(10.0f, 100.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Chopper.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Chopper.AddComponent<FSpriteComponent>("Chopper-Image", 32, 32, 0, 0, ERenderLayer::Player, 0);
    Chopper.AddComponent<F2DBoxColliderComponent>(32, 32);
    Chopper.AddComponent<FAnimationComponent>(2, 15, true);
    Chopper.AddComponent<FPlayerControllerComponent>(glm::vec2(0.0f, -80.0f), glm::vec2(80.0f, 0.0f), glm::vec2(0.0f, 80.0f), glm::vec2(-80.0f, 0.0f));
    Chopper.AddComponent<FCameraFollowComponent>();
    Chopper.AddComponent<FHealthComponent>(100);
    Chopper.AddComponent<FProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
    
    
    Entity Radar = ECSManagerInstance->CreateEntity();
    Radar.AddComponent<FTransformComponent>(glm::vec2(WindowWidth - 74.0f, 10), glm::vec2(1.0f, 1.0f), 0.0f);
    Radar.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Radar.AddComponent<FSpriteComponent>("Radar-Image", 64, 64, 0, 0, ERenderLayer::UI, 0, true);
    Radar.AddComponent<FAnimationComponent>(8, 5, true);
    
    Entity Tank = ECSManagerInstance->CreateEntity();
    Tank.Group("Enemies");
    Tank.AddComponent<FTransformComponent>(glm::vec2(500.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Tank.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Tank.AddComponent<FSpriteComponent>("Tank-Image", 32, 32, 0, 0, ERenderLayer::Enemy, 0);
    Tank.AddComponent<F2DBoxColliderComponent>(32, 32);
    Tank.AddComponent<FProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 3000, 10, false);
    Tank.AddComponent<FHealthComponent>(100);
    
    Entity Truck = ECSManagerInstance->CreateEntity();
    Truck.Group("Enemies");
    Truck.AddComponent<FTransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
    Truck.AddComponent<FRigidBodyComponent>(glm::vec2(0.0f, 0.0f));
    Truck.AddComponent<FSpriteComponent>("Truck-Image", 32, 32, 0, 0, ERenderLayer::Player, 0);
    Truck.AddComponent<F2DBoxColliderComponent>(32, 32);
    Truck.AddComponent<FProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 5000, 10, false);
    Truck.AddComponent<FHealthComponent>(100);
    
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
            if (SDLEvent.key.keysym.sym == SDLK_d)
            {
                bIsDebug = !bIsDebug;
            }
            EventManagerInstance->BroadcastEvent<KeyPressedEvent>(SDLEvent.key.keysym.sym);
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
    
    //Reset all event
    EventManagerInstance->ClearSubscribers();
    
    // Subscribe to the events of all systems
    ECSManagerInstance->GetSystem<DamageSystem>().SubscribeToEvents(EventManagerInstance);
    ECSManagerInstance->GetSystem<KeyboardControlSystem>().SubscribeToEvent(EventManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(EventManagerInstance);
    
    // Update the manager to process the entities that are waiting to be created/deleted
    ECSManagerInstance->Update();
    
    // Ask all the systems to update
    ECSManagerInstance->GetSystem<MovementSystem>().Update(DeltaTime);
    ECSManagerInstance->GetSystem<CameraMovementSystem>().Update(Camera);   
    ECSManagerInstance->GetSystem<AnimationSystem>().Update();
    ECSManagerInstance->GetSystem<CollisionSystem2D>().Update(EventManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileEmitSystem>().Update(ECSManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileLifeCycleSystem>().Update();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
    SDL_RenderClear(Renderer);
    
    // Ask all the systems that need to render
    ECSManagerInstance->GetSystem<RenderSystem>().Update(Renderer, AssetManagerInstance, Camera);
    
    if (bIsDebug)
    {
        ECSManagerInstance->GetSystem<RenderColliderSystem2D>().Update(Renderer, Camera);
    }

    SDL_RenderPresent(Renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}
