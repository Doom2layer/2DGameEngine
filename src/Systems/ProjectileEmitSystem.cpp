#include "ProjectileEmitSystem.h"

#include "../Components/2DBoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Ecs/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"

ProjectileEmitSystem::ProjectileEmitSystem()
{
    RequireComponent<FProjectileEmitterComponent>();
    RequireComponent<FTransformComponent>();
}

void ProjectileEmitSystem::Update(std::unique_ptr<ECSManager>& ECSManagerInstance)
{
    for (Entity InEntity : GetSystemEntities())
    {
        const FTransformComponent& TransformComponent = InEntity.GetComponent<FTransformComponent>();
        FProjectileEmitterComponent& ProjectileEmitterComponent = InEntity.GetComponent<FProjectileEmitterComponent>();
        
        // Check if it's time to re-emit a new projectile
        if (SDL_GetTicks() - ProjectileEmitterComponent.LastEmissionTime > ProjectileEmitterComponent.FireRate)
        {
            glm::vec2 ProjectilePosition = TransformComponent.Position;
            if (InEntity.HasComponent<FSpriteComponent>())
            {
                const FSpriteComponent& SpriteComponent = InEntity.GetComponent<FSpriteComponent>();
                ProjectilePosition.x += TransformComponent.Scale.x * SpriteComponent.SourceRectangle.w / 2;
                ProjectilePosition.y += TransformComponent.Scale.y * SpriteComponent.SourceRectangle.h / 2;
            }
            
            //Add new projectile entity to the registry
            Entity Projectile = ECSManagerInstance->CreateEntity();
            Projectile.AddComponent<FTransformComponent>(ProjectilePosition, glm::vec2(1.0f, 1.0f), 0.0f);
            Projectile.AddComponent<FRigidBodyComponent>(ProjectileEmitterComponent.Velocity);
            Projectile.AddComponent<FSpriteComponent>("Bullet-Image", 4, 4, 0, 0, ERenderLayer::Enemy, 5);
            Projectile.AddComponent<F2DBoxColliderComponent>(4, 4, glm::vec2(0.0f, 0.0f));
            Projectile.AddComponent<FProjectileComponent>(ProjectileEmitterComponent.Duration, ProjectileEmitterComponent.HitPercentDamage, ProjectileEmitterComponent.bIsFriendly);
            
            //update the Projectile emitter component last emission to the current millisec
            ProjectileEmitterComponent.LastEmissionTime = SDL_GetTicks();
            
        }
        
    }
}
