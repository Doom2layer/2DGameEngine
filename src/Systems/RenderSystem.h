#pragma once
#include <SDL_render.h>

#include "../ECS/ECS.h"

class RenderSystem : public System
{
public:
    RenderSystem();
    void Update(SDL_Renderer* Renderer);
};
