#include "CameraMovementSystem.h"

#include <SDL_rect.h>

#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../Game/Game.h"

CameraMovementSystem::CameraMovementSystem()
{
    RequireComponent<FCameraFollowComponent>();
    RequireComponent<FTransformComponent>();
}

void CameraMovementSystem::Update(SDL_Rect& Camera)
{
    for (Entity InEntity : GetSystemEntities())
    {
        FTransformComponent& TransformComponent = InEntity.GetComponent<FTransformComponent>();
        
        if (TransformComponent.Position.x + (Camera.w / 2) < Game::MapWidth) Camera.x = TransformComponent.Position.x - (Game::WindowWidth / 2);

        if (TransformComponent.Position.y + (Camera.h / 2) < Game::MapHeight) Camera.y = TransformComponent.Position.y - (Game::WindowHeight / 2);
        
        // Keep camera rectangle view inside the screen limits 
        Camera.x = Camera.x < 0 ? 0 : Camera.x;
        Camera.y = Camera.y < 0 ? 0 : Camera.y;
        Camera.x = Camera.x > Camera.w ? Camera.w : Camera.x;
        Camera.y = Camera.y > Camera.h ? Camera.h : Camera.y;
    }
}
