#pragma once
#include <SDL.h>
#include "../ECS/ECS.h"

class RenderGUISystem : public System
{
public:
    RenderGUISystem() = default;
    
    void Update(const std::unique_ptr<ECSManager>& ECSManagerInstance, const SDL_Rect& Camera);
};
