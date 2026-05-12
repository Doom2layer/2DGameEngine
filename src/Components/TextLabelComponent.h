#pragma once
#include <SDL_pixels.h>

#include "../ECS/ECS.h"
#include "glm/vec2.hpp"

struct FTextLabelComponent : Component<FTextLabelComponent>
{
    glm::vec2 Position;
    std::string Text;
    std::string AssetID;
    SDL_Color Color;
    bool bIsFixed;
    
    FTextLabelComponent(glm::vec2 InPosition = glm::vec2{0.0, 0.0}, std::string InText = "", std::string InAssetID = "", const SDL_Color InColor = {0, 0, 0}, bool bInIsFixed = true) : Position(InPosition), Text(InText), AssetID(InAssetID), Color(InColor), bIsFixed(bInIsFixed)
    {
    }
};
