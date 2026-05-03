#pragma once
#include <SDL_rect.h>

#include "../ECS/ECS.h"

enum class ERenderLayer : uint8_t
{
    Background  = 0,
    Vegetation  = 1,
    Obstacle    = 2,
    Enemy       = 3,
    Player      = 4,
    UI          = 5
};

struct FSpriteComponent : public Component<FSpriteComponent>
{
    std::string     AssetID;
    SDL_Rect        SourceRectangle;
    ERenderLayer    RenderLayer;
    int             ZIndex;

    // Full constructor — for spritesheets and specific layer ordering
    FSpriteComponent(
        const std::string& InAssetID  = "",
        int InWidth                   = 0,
        int InHeight                  = 0,
        int InSourceX                 = 0,
        int InSourceY                 = 0,
        ERenderLayer InLayer          = ERenderLayer::Background,
        int InZIndex                  = 0)
        : AssetID(InAssetID)
        , SourceRectangle({InSourceX, InSourceY, InWidth, InHeight})
        , RenderLayer(InLayer)
        , ZIndex(InZIndex)
    {}
    
};