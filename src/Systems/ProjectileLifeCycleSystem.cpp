#include "ProjectileLifeCycleSystem.h"

#include "../Components/ProjectileComponent.h"

ProjectileLifeCycleSystem::ProjectileLifeCycleSystem()
{
    RequireComponent<FProjectileComponent>();
}

void ProjectileLifeCycleSystem::Update()
{
    for (Entity InEntity : GetSystemEntities())
    {
        FProjectileComponent& Projectile = InEntity.GetComponent<FProjectileComponent>();
        // Kill Projectile after they reach duration limit
        if (SDL_GetTicks() - Projectile.StartTime > Projectile.Duration) InEntity.Kill();
    }
}
