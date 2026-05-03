#pragma once
#include <glm/glm.hpp>
#include "../ECS/ECS.h"

struct F2DBoxColliderComponent: public Component<F2DBoxColliderComponent>
{
    float Width;
    float Height;
    glm::vec2 Offset;
    
    F2DBoxColliderComponent(float InWidth = 0, float InHeight = 0, glm::vec2 InOffset = glm::vec2{0.0, 0.0}) : Width(InWidth), Height(InHeight), Offset(InOffset) {}
};
