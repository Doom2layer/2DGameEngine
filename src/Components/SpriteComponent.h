#pragma once
#include <SDL_rect.h>
#include <SDL_render.h>

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
    std::string         AssetID;
    SDL_Rect            SourceRectangle;
    SDL_RendererFlip    Flip;
    ERenderLayer        RenderLayer;
    int                 ZIndex;
    bool                bIsFixed;

    // Full constructor — for spritesheets and specific layer ordering
    FSpriteComponent(
        const std::string& InAssetID  = "",
        int InWidth                   = 0,
        int InHeight                  = 0,
        int InSourceX                 = 0,
        int InSourceY                 = 0,
        ERenderLayer InLayer          = ERenderLayer::Background,
        int InZIndex                  = 0,
        bool InIsFixed                = false,
        SDL_RendererFlip InFlip       = SDL_FLIP_NONE
        )
        : AssetID(InAssetID)
        , SourceRectangle({InSourceX, InSourceY, InWidth, InHeight})
        , RenderLayer(InLayer)
        , ZIndex(InZIndex)
        , bIsFixed(InIsFixed)
        , Flip(InFlip)
    {}
    
};