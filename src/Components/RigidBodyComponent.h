#pragma once
#include "glm/vec2.hpp"
#include "../ECS/ECS.h"

struct FRigidBodyComponent : public Component<FRigidBodyComponent>
{
    glm::vec2 Velocity;
    
    FRigidBodyComponent(glm::vec2 InVelocity = glm::vec2{0.0, 0.0}) : Velocity(InVelocity) {}
};
