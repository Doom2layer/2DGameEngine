#pragma once
#include <SDL_render.h>

#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"

class RenderSystem : public System
{
public:
    RenderSystem();
    void Update(SDL_Renderer* Renderer, const std::unique_ptr<AssetManager>& AssetManagerInstance);
    void MakeDirty();
    void AddEntityToSystem(Entity InEntity) override;

private: 
    // true to initially sort on first frame
    bool bIsSortDirty = true; 
};
