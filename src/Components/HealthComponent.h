#pragma once

#include "../ECS/ECS.h"

struct FHealthComponent : public Component<FHealthComponent>
{
    int HealthPercentage;
    
    FHealthComponent(int InHealthPercentage = 0) : HealthPercentage(InHealthPercentage) {}
};
