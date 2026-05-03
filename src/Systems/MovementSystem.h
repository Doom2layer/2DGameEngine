#pragma once
#include "../ECS/ECS.h"

class MovementSystem : public System
{
public:
    MovementSystem();
    void Update(double DeltaTime);
};
