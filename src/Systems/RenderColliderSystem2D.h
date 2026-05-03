#pragma once
#include "../ECS/ECS.h"
#include <SDL.h>

class RenderColliderSystem2D: public System
{
public:
    RenderColliderSystem2D();
    void Update(SDL_Renderer* Renderer);
    
    /*
    void DrawCircle(SDL_Renderer* Renderer, int CenterX, int CenterY, int Radius);
    */
    
};
