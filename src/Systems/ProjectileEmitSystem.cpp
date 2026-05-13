#include "ProjectileEmitSystem.h"

#include "../Components/2DBoxColliderComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Ecs/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventManager/EventManager.h"

ProjectileEmitSystem::ProjectileEmitSystem()
{
    RequireComponent<FProjectileEmitterComponent>();
    RequireComponent<FTransformComponent>();
}

void ProjectileEmitSystem::SubscribeToEvents(const std::unique_ptr<EventManager>& InEventManager)
{
    InEventManager->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
}

void ProjectileEmitSystem::OnKeyPressed(KeyPressedEvent& Event)
{
    if (Event.KeyCode == SDLK_z)
    {
        for (Entity InEntity : GetSystemEntities())
        {
            if (InEntity.HasComponent<FCameraFollowComponent>())
            {
                const FProjectileEmitterComponent& ProjectileEmitterComponent = InEntity.GetComponent<FProjectileEmitterComponent>();
                const FTransformComponent& TransformComponent = InEntity.GetComponent<FTransformComponent>();
                const FRigidBodyComponent& RigidBodyComponent = InEntity.GetComponent<FRigidBodyComponent>();
                
                // if parent has sprite component, start the projectile position in the middle of the sprite, otherwise start at the transform position
                glm::vec2 ProjectilePosition = TransformComponent.Position;
                if (InEntity.HasComponent<FSpriteComponent>())
                {
                    const FSpriteComponent& SpriteComponent = InEntity.GetComponent<FSpriteComponent>();
                    ProjectilePosition.x += TransformComponent.Scale.x * SpriteComponent.SourceRectangle.w / 2;
                    ProjectilePosition.y += TransformComponent.Scale.y * SpriteComponent.SourceRectangle.h / 2;
                }
                
                // if the parent is controlled by keyboard keys, modify the projectile velocity direction based on the parent velocity direction, otherwise use the projectile emitter velocity as is
                glm::vec2 ProjectileVelocity = ProjectileEmitterComponent.Velocity;
                float DirectionX = 0;
                float DirectionY = 0;
                if (RigidBodyComponent.Velocity.x > 0) DirectionX = +1;
                if (RigidBodyComponent.Velocity.x < 0) DirectionX = -1;
                if (RigidBodyComponent.Velocity.y > 0) DirectionY = +1;
                if (RigidBodyComponent.Velocity.y < 0) DirectionY = -1;
                ProjectileVelocity.x = ProjectileEmitterComponent.Velocity.x * DirectionX;
                ProjectileVelocity.y = ProjectileEmitterComponent.Velocity.y * DirectionY;
                
                // Create new projectile entity and add it to the world
                Entity Projectile = InEntity.Manager->CreateEntity();
                Projectile.Group("Projectiles");
                Projectile.AddComponent<FTransformComponent>(ProjectilePosition, glm::vec2(1.0f, 1.0f), 0.0f);
                Projectile.AddComponent<FRigidBodyComponent>(ProjectileVelocity);
                Projectile.AddComponent<FSpriteComponent>("bullet-texture", 4, 4, 0, 0, ERenderLayer::Player, 5);
                Projectile.AddComponent<F2DBoxColliderComponent>(4, 4, glm::vec2(0.0f, 0.0f));
                Projectile.AddComponent<FProjectileComponent>(ProjectileEmitterComponent.Duration, ProjectileEmitterComponent.HitPercentDamage, ProjectileEmitterComponent.bIsFriendly);
            }
        }
    }
}

void ProjectileEmitSystem::Update(const std::unique_ptr<ECSManager>& ECSManagerInstance)
{
    for (Entity InEntity : GetSystemEntities())
    {
        const FTransformComponent& TransformComponent = InEntity.GetComponent<FTransformComponent>();
        FProjectileEmitterComponent& ProjectileEmitterComponent = InEntity.GetComponent<FProjectileEmitterComponent>();
        
        if (ProjectileEmitterComponent.FireRate == 0) continue;
        
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
            Projectile.Group("Projectiles");
            Projectile.AddComponent<FTransformComponent>(ProjectilePosition, glm::vec2(1.0f, 1.0f), 0.0f);
            Projectile.AddComponent<FRigidBodyComponent>(ProjectileEmitterComponent.Velocity);
            Projectile.AddComponent<FSpriteComponent>("bullet-texture", 4, 4, 0, 0, ERenderLayer::Enemy, 5);
            Projectile.AddComponent<F2DBoxColliderComponent>(4, 4, glm::vec2(0.0f, 0.0f));
            Projectile.AddComponent<FProjectileComponent>(ProjectileEmitterComponent.Duration, ProjectileEmitterComponent.HitPercentDamage, ProjectileEmitterComponent.bIsFriendly);
            
            //update the Projectile emitter component last emission to the current millisec
            ProjectileEmitterComponent.LastEmissionTime = SDL_GetTicks();
            
        }
        
    }
}
