#pragma once

#include <SDL.h>
#include <memory>
#include <optional>
#include <string>

#include "../ECS/ECS.h"
#include "Panels/Inspector/InspectorPanel.h"
#include "Panels/ViewPort/ViewPortPanel.h"
#include "Panels/WorldOutliner/WorldOutlinerPanel.h"

class Editor
{
public:
  enum class PlayState
  {
    Stopped,
    Playing,
    Paused
  };

  Editor() = default;
  ~Editor();

  void Initialize(SDL_Renderer* renderer, SDL_Window* window, ECSManager* ecsManager);
  void Update();
  void Render();
  void Shutdown();

  bool IsInitialized() const { return bIsInitialized; }
  ECSManager* GetECSManager() const { return ECSManagerInstance; }
  PlayState GetPlayState() const { return CurrentPlayState; }
  void SetPlayState(PlayState NewState);
  void SetCamera(const SDL_Rect& NewCamera) { Camera = NewCamera; }
  bool HasSelectedEntity() const { return SelectedEntity.has_value(); }
  Entity GetSelectedEntity() const { return SelectedEntity.value(); }
  std::string GetSelectedEntityTag() const;
  void SelectEntity(const Entity& InEntity);
  void SelectEntityAtScreenPoint(int ScreenX, int ScreenY);
  bool TryGetSelectedEntityScreenRect(SDL_Rect& OutRect) const;
  void ClearSelection();
  void RenameSelectedEntity(const std::string& NewName);
  void SetECSManager(ECSManager* NewECSManager) { ECSManagerInstance = NewECSManager; }
  void SetViewportTexture(SDL_Texture* NewViewportTexture) { ViewportTexture = NewViewportTexture; }
  SDL_Texture* GetViewportTexture() const { return ViewportTexture; }

private:
  SDL_Renderer* Renderer{ nullptr };
  SDL_Window* Window{ nullptr };
  ECSManager* ECSManagerInstance{ nullptr };
  SDL_Texture* ViewportTexture{ nullptr };
  SDL_Rect Camera{};
  bool bIsInitialized{ false };
  PlayState CurrentPlayState{ PlayState::Stopped };
  std::optional<Entity> SelectedEntity;
  std::unique_ptr<ViewPortPanel> Viewport;
  std::unique_ptr<WorldOutlinerPanel> WorldOutliner;
  std::unique_ptr<InspectorPanel> Inspector;
};
