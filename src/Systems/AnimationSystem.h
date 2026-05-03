#pragma once
#include "../ECS/ECS.h"

class AnimationSystem : public System
{
public:
    AnimationSystem();
    void AddEntityToSystem(Entity InEntity) override;
    void Update();
    
    
};
