#pragma once
#include "../ECS/ECS.h"
#include "SDL.h"
class AssetManager;

class RenderHealthBarSystem : public System
{
public:
    RenderHealthBarSystem();
    void Update(SDL_Renderer* Renderer, const SDL_Rect& Camera, std::unique_ptr<AssetManager>& AssetManagerInstance);
};
