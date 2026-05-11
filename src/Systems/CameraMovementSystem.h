#pragma once
#include <SDL_rect.h>

#include "../ECS/ECS.h"

class CameraMovementSystem : public System
{
public:
    CameraMovementSystem();
    void Update(SDL_Rect& Camera);
};
