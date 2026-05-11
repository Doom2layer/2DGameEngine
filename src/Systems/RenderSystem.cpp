#include "RenderSystem.h"

#include <algorithm>
#include <SDL_rect.h>
#include <SDL_render.h>

#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"

RenderSystem::RenderSystem()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<FSpriteComponent>();
}

void RenderSystem::AddEntityToSystem(Entity InEntity)
{
    System::AddEntityToSystem(InEntity);
    // We override the base AddEntityToSystem function to mark the render system as dirty whenever a new entity is added, so that the entities will be sorted on the next update.
    MakeDirty();
}

void RenderSystem::Update(SDL_Renderer* Renderer, const std::unique_ptr<AssetManager>& AssetManagerInstance, SDL_Rect& Camera)
{
    std::vector<Entity> Entities = GetSystemEntities();
    
    // Using bIsSortDirty to only sort the entities when necessary (when a new entity is added or an entity is removed), to avoid unnecessary sorting every frame which can be expensive if we have a lot of entities.
    if (bIsSortDirty)
    {
        std::sort(Entities.begin(), Entities.end(), [](const Entity& A, const Entity& B)
        {
            const FSpriteComponent& SpriteA = A.GetComponent<FSpriteComponent>();
            const FSpriteComponent& SpriteB = B.GetComponent<FSpriteComponent>();
            
            // Sort by layer first
            if (SpriteA.RenderLayer != SpriteB.RenderLayer)
            {
                return static_cast<uint8_t>(SpriteA.RenderLayer) < static_cast<uint8_t>(SpriteB.RenderLayer);
            }
            
            // if in the same layer they have different ZIndex 
            return SpriteA.ZIndex < SpriteB.ZIndex;
        });
        bIsSortDirty = false;
    }
    
    // Loop all the entities that the system is interested in
    for (const Entity& InEntity : Entities)
    {
        const FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FSpriteComponent& Sprite = InEntity.GetComponent<FSpriteComponent>();
        
        // Set the source rectangle of our original sprite texture
        SDL_Rect SourceRect = Sprite.SourceRectangle;
        
        // Set the destination rectangle with the x,y position to be rendered
        SDL_Rect DestinationRect = {
            static_cast<int>(Transform.Position.x - (Sprite.bIsFixed ? 0 : Camera.x)), 
            static_cast<int>(Transform.Position.y - (Sprite.bIsFixed ? 0 : Camera.y)),
            static_cast<int>(Sprite.SourceRectangle.w * Transform.Scale.x), 
            static_cast<int>(Sprite.SourceRectangle.h * Transform.Scale.y)
        };
        
        SDL_RenderCopyEx(Renderer, AssetManagerInstance->GetTexture(Sprite.AssetID), &SourceRect, &DestinationRect, Transform.Rotation, NULL, SDL_FLIP_NONE);
        
    }
}

void RenderSystem::MakeDirty()
{
    bIsSortDirty = true;
}
