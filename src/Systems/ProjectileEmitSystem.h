#pragma once
#include "../ECS/ECS.h"

class ECSManager;

class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem();
    void Update(std::unique_ptr<ECSManager>& ECSManagerInstance);
    
};
