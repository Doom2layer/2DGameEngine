#pragma once
#include "../ECS/ECS.h"
#include "SDL.h"
class AssetManager;

class RenderTextSystem : public System
{
public:
    RenderTextSystem();
    void Update(SDL_Renderer* Renderer, const SDL_Rect& Camera, std::unique_ptr<AssetManager>& AssetManagerInstance);
    
};
