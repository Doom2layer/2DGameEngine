#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Editor/Editor.h"
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
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>

#include "LevelLoader.h"
#include "../Systems/ScriptSystem.h"


int Game::WindowWidth;
int Game::WindowHeight;
int Game::MapWidth;
int Game::MapHeight;

namespace
{
    void DrawSelectionHighlight(SDL_Renderer* Renderer, const SDL_Rect& Rect)
    {
        if (!Renderer || Rect.w <= 0 || Rect.h <= 0)
        {
            return;
        }

        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Renderer, 255, 220, 40, 40);
        SDL_RenderFillRect(Renderer, &Rect);
        SDL_SetRenderDrawColor(Renderer, 255, 220, 40, 255);
        SDL_RenderDrawRect(Renderer, &Rect);
    }
}


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
    
    if (TTF_Init() != 0)
    {
        Logger::Error("TTF_Init failed: " + std::string(TTF_GetError()));
        return;
    }
    
    SDL_DisplayMode DisplayMode;
    SDL_GetCurrentDisplayMode(0, &DisplayMode);
    WindowWidth = 800;//DisplayMode.w;
    WindowHeight = 600;//DisplayMode.h;
    
    Window = SDL_CreateWindow("Mustafa's 2D Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    
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
    
    // Initialize ImGUI Context
    ImGui::CreateContext();
    ImGuiSDL::Initialize(Renderer, WindowWidth, WindowHeight);
    
    // Initialize the camera view with the entire screen area
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = WindowWidth;
    Camera.h = WindowHeight;
    
    ECSManagerInstance = std::make_unique<ECSManager>();
    AssetManagerInstance = std::make_unique<AssetManager>(Renderer);
    EventManagerInstance = std::make_unique<EventManager>();
    EditorInstance = std::make_unique<Editor>();
    EditorInstance->Initialize(Renderer, Window, ECSManagerInstance.get());

    
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
    BuildScene();
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
        //ImGui SDL Input
        ImGui_ImplSDL2_ProcessEvent(&SDLEvent);
        ImGuiIO& IO = ImGui::GetIO();
        
        int MouseX, MouseY;
        const Uint32 Buttons = SDL_GetMouseState(&MouseX, &MouseY);
        
        IO.MousePos = ImVec2(static_cast<float>(MouseX), static_cast<float>(MouseY));
        IO.MouseDown[0] = (Buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        IO.MouseDown[1] = (Buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        
        // Handle Core SDL Event (close window, key pressed, etc.)
        switch (SDLEvent.type)
        {
            case SDL_QUIT:
                bIsRunning = false;
            break;

            case SDL_WINDOWEVENT:
                if (SDLEvent.window.event == SDL_WINDOWEVENT_RESIZED || SDLEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    WindowWidth = SDLEvent.window.data1;
                    WindowHeight = SDLEvent.window.data2;
                    Camera.w = WindowWidth;
                    Camera.h = WindowHeight;
                }
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

            default:
            break;
        }
    }
}

void Game::Update()
{
    const Editor::PlayState PlayState = EditorInstance ? EditorInstance->GetPlayState() : Editor::PlayState::Playing;

    if (PlayState == Editor::PlayState::Stopped && LastEditorPlayState != Editor::PlayState::Stopped)
    {
        BuildScene();
        LastEditorPlayState = PlayState;
        return;
    }

    LastEditorPlayState = PlayState;

    if (PlayState != Editor::PlayState::Playing)
    {
        MilliSecondsPreviousFrame = static_cast<int>(SDL_GetTicks());
        return;
    }

    // if we are running too fast, we waste some time until we reach the MILLISECONDS_PER_FRAME
    int TimeToWait = MILLISECONDS_PER_FRAME - static_cast<int>(SDL_GetTicks() - static_cast<Uint32>(MilliSecondsPreviousFrame));
    
    if (TimeToWait > 0 && TimeToWait <= MILLISECONDS_PER_FRAME)
    {
        SDL_Delay(TimeToWait);
    }
    
    // The difference in ticks since last frame, converted into seconds
    double DeltaTime = (SDL_GetTicks() - MilliSecondsPreviousFrame) / 1000.0;
    
    // Store the current frame time
    MilliSecondsPreviousFrame = static_cast<int>(SDL_GetTicks());
    
    //Reset all event
    EventManagerInstance->ClearSubscribers();
    
    // Subscribe to the events of all systems
    ECSManagerInstance->GetSystem<DamageSystem>().SubscribeToEvents(EventManagerInstance);
    ECSManagerInstance->GetSystem<KeyboardControlSystem>().SubscribeToEvent(EventManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(EventManagerInstance);
    ECSManagerInstance->GetSystem<MovementSystem>().SubscribeToEvents(EventManagerInstance);
    
    // Update the manager to process the entities that are waiting to be created/deleted
    ECSManagerInstance->Update();
    
    // Ask all the systems to update
    ECSManagerInstance->GetSystem<MovementSystem>().Update(DeltaTime);
    ECSManagerInstance->GetSystem<CameraMovementSystem>().Update(Camera);   
    ECSManagerInstance->GetSystem<AnimationSystem>().Update();
    ECSManagerInstance->GetSystem<CollisionSystem2D>().Update(EventManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileEmitSystem>().Update(ECSManagerInstance);
    ECSManagerInstance->GetSystem<ProjectileLifeCycleSystem>().Update();
    ECSManagerInstance->GetSystem<ScriptSystem>().Update(DeltaTime, MilliSecondsPreviousFrame);
}

void Game::Render()
{
    SDL_GetWindowSize(Window, &WindowWidth, &WindowHeight);
    ImGuiIO& IO = ImGui::GetIO();
    IO.DisplaySize = ImVec2(static_cast<float>(WindowWidth), static_cast<float>(WindowHeight));

    if (EditorInstance)
    {
        EditorInstance->SetCamera(Camera);
    }

    EnsureViewportTexture();
    if (EditorInstance)
    {
        EditorInstance->SetViewportTexture(ViewportTexture);
    }

    const bool bRenderToTexture = (ViewportTexture != nullptr);

    if (bRenderToTexture)
    {
        SDL_SetRenderTarget(Renderer, ViewportTexture);
        SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
        SDL_RenderClear(Renderer);
    }
    else
    {
        SDL_SetRenderTarget(Renderer, nullptr);
        SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
        SDL_RenderClear(Renderer);
    }

    ImGui::NewFrame();
    
    // Ask all the systems that need to render
    ECSManagerInstance->GetSystem<RenderSystem>().Update(Renderer, AssetManagerInstance, Camera);
    ECSManagerInstance->GetSystem<RenderTextSystem>().Update(Renderer, Camera, AssetManagerInstance);
    ECSManagerInstance->GetSystem<RenderHealthBarSystem>().Update(Renderer, Camera, AssetManagerInstance);
    
    if (bIsDebug)
    {
        ECSManagerInstance->GetSystem<RenderColliderSystem2D>().Update(Renderer, Camera);
    }

    if (EditorInstance)
    {
        SDL_Rect SelectedRect{};
        if (EditorInstance->TryGetSelectedEntityScreenRect(SelectedRect))
        {
            DrawSelectionHighlight(Renderer, SelectedRect);
        }
    }

    if (bRenderToTexture)
    {
        SDL_SetRenderTarget(Renderer, nullptr);
        SDL_SetRenderDrawColor(Renderer, 21, 21, 21, 255);
        SDL_RenderClear(Renderer);
    }

    if (EditorInstance && EditorInstance->IsInitialized())
    {
        EditorInstance->Render();
    }

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    SDL_RenderPresent(Renderer);
}

void Game::Destroy()
{
    if (EditorInstance)
    {
        EditorInstance->Shutdown();
    }

    DestroyViewportTexture();
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void Game::BuildScene()
{
    if (AssetManagerInstance)
    {
        AssetManagerInstance->ClearAsset();
    }

    ECSManagerInstance = std::make_unique<ECSManager>();
    EventManagerInstance = std::make_unique<EventManager>();
    LuaState = sol::state{};

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
    ECSManagerInstance->AddSystem<RenderTextSystem>();
    ECSManagerInstance->AddSystem<RenderHealthBarSystem>();
    ECSManagerInstance->AddSystem<ScriptSystem>();

    ECSManagerInstance->GetSystem<ScriptSystem>().CreateLuaBindings(LuaState);
    LuaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::os);

    LevelLoader Loader;
    Loader.LoadLevel(LuaState, ECSManagerInstance, AssetManagerInstance, 1);
    ECSManagerInstance->Update();

    if (EditorInstance)
    {
        EditorInstance->SetECSManager(ECSManagerInstance.get());
        EditorInstance->ClearSelection();
    }

    Camera.x = 0;
    Camera.y = 0;
    Camera.w = WindowWidth;
    Camera.h = WindowHeight;
    MilliSecondsPreviousFrame = static_cast<int>(SDL_GetTicks());
}

void Game::EnsureViewportTexture()
{
    if (WindowWidth <= 0 || WindowHeight <= 0 || !Renderer)
    {
        DestroyViewportTexture();
        return;
    }

    if (ViewportTexture)
    {
        int ExistingWidth = 0;
        int ExistingHeight = 0;
        if (SDL_QueryTexture(ViewportTexture, nullptr, nullptr, &ExistingWidth, &ExistingHeight) == 0 &&
            ExistingWidth == WindowWidth && ExistingHeight == WindowHeight)
        {
            return;
        }

        DestroyViewportTexture();
    }

    ViewportTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight);
    if (!ViewportTexture)
    {
        Logger::Error("Failed to create viewport render target: " + std::string(SDL_GetError()));
        return;
    }

    SDL_SetTextureBlendMode(ViewportTexture, SDL_BLENDMODE_BLEND);
}

void Game::DestroyViewportTexture()
{
    if (ViewportTexture)
    {
        SDL_DestroyTexture(ViewportTexture);
        ViewportTexture = nullptr;
    }
}

