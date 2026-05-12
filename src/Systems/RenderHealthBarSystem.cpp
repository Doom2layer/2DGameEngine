#include "RenderHealthBarSystem.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "../AssetManager/AssetManager.h"
#include "../Components/HealthComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"

RenderHealthBarSystem::RenderHealthBarSystem()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<FSpriteComponent>();
    RequireComponent<FHealthComponent>();
}

void RenderHealthBarSystem::Update(SDL_Renderer* Renderer, const SDL_Rect& Camera,
    std::unique_ptr<AssetManager>& AssetManagerInstance)
{
    constexpr int HealthBarMaxWidth = 15;
    constexpr int HealthBarHeight   = 3;
    constexpr int LabelOffsetX      = 5;

    for (Entity InEntity : GetSystemEntities())
    {
        const FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FSpriteComponent&    Sprite    = InEntity.GetComponent<FSpriteComponent>();
        const FHealthComponent&    Health    = InEntity.GetComponent<FHealthComponent>();

        // Determine bar color based on health percentage
        SDL_Color HealthBarColor;
        if      (Health.HealthPercentage <= 40) { HealthBarColor = { 255, 0,   0,   255 }; }
        else if (Health.HealthPercentage <= 80) { HealthBarColor = { 255, 255, 0,   255 }; }
        else                                    { HealthBarColor = { 0,   255, 0,   255 }; }

        // Position
        const double BarX = (Transform.Position.x + (Sprite.SourceRectangle.w * Transform.Scale.x)) - Camera.x;
        const double BarY =  Transform.Position.y - Camera.y;

        // Draw health bar
        const SDL_Rect HealthBarRect = {
            static_cast<int>(BarX),
            static_cast<int>(BarY),
            static_cast<int>(HealthBarMaxWidth * (Health.HealthPercentage / 100.0)),
            HealthBarHeight
        };
        SDL_SetRenderDrawColor(Renderer, HealthBarColor.r, HealthBarColor.g, HealthBarColor.b, HealthBarColor.a);
        SDL_RenderFillRect(Renderer, &HealthBarRect);

        // Render health percentage label — ideally cached in FHealthComponent
        const std::string HealthText = std::to_string(Health.HealthPercentage) + "%";
        SDL_Surface* Surface = TTF_RenderText_Blended(
            AssetManagerInstance->GetFont("Arial-Font"), HealthText.c_str(), HealthBarColor
        );
        if (!Surface) continue;

        SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
        SDL_FreeSurface(Surface);
        if (!Texture) continue;

        int LabelWidth = 0, LabelHeight = 0;
        SDL_QueryTexture(Texture, nullptr, nullptr, &LabelWidth, &LabelHeight);

        const SDL_Rect LabelRect = {
            static_cast<int>(BarX + HealthBarMaxWidth + LabelOffsetX),
            static_cast<int>(BarY + (HealthBarHeight / 2) - (LabelHeight / 2)),
            LabelWidth,
            LabelHeight
        };

        SDL_RenderCopy(Renderer, Texture, nullptr, &LabelRect);
        SDL_DestroyTexture(Texture);
    }
}