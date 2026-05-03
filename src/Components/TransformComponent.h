#pragma once
#include "../ECS/ECS.h"
#include "glm/vec2.hpp"

struct FTransformComponent : public Component<FTransformComponent>
{
    glm::vec2 Position;
    glm::vec2 Scale;
    float Rotation;
    
    FTransformComponent(glm::vec2 InPosition = glm::vec2{0.0, 0.0}, glm::vec2 InScale = glm::vec2{1.0, 1.0}, float InRotation = 0.0f) : Position(InPosition), Scale(InScale), Rotation(InRotation) {}
};
