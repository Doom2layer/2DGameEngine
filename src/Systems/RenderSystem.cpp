#include "RenderSystem.h"

#include <SDL_rect.h>
#include <SDL_render.h>

#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"

RenderSystem::RenderSystem()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<FSpriteComponent>();
}

void RenderSystem::Update(SDL_Renderer* Renderer)
{
    // Loop all the entities that the system is interested in
    for (const Entity& InEntity : GetSystemEntities())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FSpriteComponent& Sprite = InEntity.GetComponent<FSpriteComponent>();
        
        SDL_Rect RenderRect = {
            static_cast<int>(Transform.Position.x),
            static_cast<int>(Transform.Position.y),
            Sprite.Width,
            Sprite.Height
        };
        
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255); // Set color to red
        SDL_RenderFillRect(Renderer, &RenderRect);
        
    }
}
