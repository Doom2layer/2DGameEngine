#pragma once
#include "../ECS/ECS.h"
#include "glm/vec2.hpp"

struct FSpriteComponent : public Component<FSpriteComponent>
{
    int Width;
    int Height;
    
    FSpriteComponent(int InWidth = 0, int InHeight = 0) : Width(InWidth), Height(InHeight){}
    
};
