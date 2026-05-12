#include "RenderTextSystem.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "../AssetManager/AssetManager.h"
#include "../Components/TextLabelComponent.h"

RenderTextSystem::RenderTextSystem()
{
    RequireComponent<FTextLabelComponent>();
}

void RenderTextSystem::Update(SDL_Renderer* Renderer, const SDL_Rect& Camera, std::unique_ptr<AssetManager>& AssetManagerInstance)
{
    for (Entity InEntity : GetSystemEntities())
    {
        const FTextLabelComponent& TextLabel = InEntity.GetComponent<FTextLabelComponent>();

        SDL_Surface* Surface = TTF_RenderText_Blended(
            AssetManagerInstance->GetFont(TextLabel.AssetID),
            TextLabel.Text.c_str(),
            TextLabel.Color
        );
        if (!Surface)
        {
            Logger::Error("Failed to create surface: " + std::string(TTF_GetError()));
            continue;
        }

        SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
        SDL_FreeSurface(Surface);
        if (!Texture)
        {
            Logger::Error("Failed to create texture: " + std::string(SDL_GetError()));
            continue;
        }

        int LabelWidth = 0, LabelHeight = 0;
        SDL_QueryTexture(Texture, nullptr, nullptr, &LabelWidth, &LabelHeight);

        const int OffsetX = TextLabel.bIsFixed ? 0 : Camera.x;
        const int OffsetY = TextLabel.bIsFixed ? 0 : Camera.y;

        const SDL_Rect DestRect = {
            static_cast<int>(TextLabel.Position.x) - OffsetX,
            static_cast<int>(TextLabel.Position.y) - OffsetY,
            LabelWidth,
            LabelHeight
        };

        SDL_RenderCopy(Renderer, Texture, nullptr, &DestRect);
        SDL_DestroyTexture(Texture);
    }
}
