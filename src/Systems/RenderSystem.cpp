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

void RenderSystem::Update(SDL_Renderer* Renderer, const std::unique_ptr<AssetManager>& AssetManagerInstance)
{
    // Loop all the entities that the system is interested in
    for (const Entity& InEntity : GetSystemEntities())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FSpriteComponent& Sprite = InEntity.GetComponent<FSpriteComponent>();
        
        // Set the source rectangle of our original sprite texture
        SDL_Rect SourceRect = Sprite.SourceRectangle;
        
        // Set the destination rectangle with the x,y position to be rendered
        SDL_Rect DestinationRect = {
            static_cast<int>(Transform.Position.x), 
            static_cast<int>(Transform.Position.y), 
            static_cast<int>(Sprite.SourceRectangle.w * Transform.Scale.x), 
            static_cast<int>(Sprite.SourceRectangle.h * Transform.Scale.y)
        };
        
        SDL_RenderCopyEx(Renderer, AssetManagerInstance->GetTexture(Sprite.AssetID), &SourceRect, &DestinationRect, Transform.Rotation, NULL, SDL_FLIP_NONE);
        
    }
}
