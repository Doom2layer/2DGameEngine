#pragma once
#include "../ECS/ECS.h"
#include <glm/glm.hpp>

struct F2DCircleColliderComponent : Component<F2DCircleColliderComponent>
{
    float Radius;
    glm::vec2 Offset;
    
    F2DCircleColliderComponent(float InRadius = 0, glm::vec2 InOffset = glm::vec2{0.0, 0.0}) : Radius(InRadius), Offset(InOffset) {}
    
};
