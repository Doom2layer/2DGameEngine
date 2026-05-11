#pragma once
#include "../ECS/ECS.h"
#include "glm/vec2.hpp"

struct FPlayerControllerComponent : public Component<FPlayerControllerComponent>
{
public:
    glm::vec2 UpVelocity;
    glm::vec2 RightVelocity;
    glm::vec2 DownVelocity;
    glm::vec2 LeftVelocity;
    
    FPlayerControllerComponent(glm::vec2 InUpVelocity = glm::vec2(0), glm::vec2 InRightVelocity = glm::vec2(0), glm::vec2 InDownVelocity = glm::vec2(0), glm::vec2 InLeftVelocity = glm::vec2(0))
    {
        UpVelocity = InUpVelocity;
        RightVelocity = InRightVelocity;
        DownVelocity = InDownVelocity;
        LeftVelocity = InLeftVelocity;
    }
};
