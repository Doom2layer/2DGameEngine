#pragma once
#include <SDL_rect.h>

#include "../ECS/ECS.h"

struct FSpriteComponent : public Component<FSpriteComponent>
{
    std::string AssetID;
    SDL_Rect SourceRectangle;

    FSpriteComponent(
        const std::string& InAssetID = "",
        int InWidth = 0,
        int InHeight = 0,
        int InSourceX = 0,
        int InSourceY = 0)
        : AssetID(InAssetID)
        , SourceRectangle({InSourceX, InSourceY, InWidth, InHeight})
    {}
};