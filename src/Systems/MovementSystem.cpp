#include "MovementSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Events/CollisionEvent.h"
#include "../Game/Game.h"

constexpr float EPSILON = std::numeric_limits<float>::epsilon();

MovementSystem::MovementSystem()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<FRigidBodyComponent>();
}

void MovementSystem::Update(double DeltaTime)
{
    // Loop all the entities that the system is interested in
    for (Entity& InEntity : GetSystemEntities())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FRigidBodyComponent& RigidBody = InEntity.GetComponent<FRigidBodyComponent>();
        Transform.Position.x += RigidBody.Velocity.x * DeltaTime;
        Transform.Position.y += RigidBody.Velocity.y * DeltaTime;
        
        if (InEntity.HasTag("Player"))
        {
            int PaddingLeft = 10;
            int PaddingRight = 10;
            int PaddingTop = 50;
            int PaddingBottom = 50;
            
            Transform.Position.x = Transform.Position.x < PaddingLeft ? PaddingLeft : Transform.Position.x;
            Transform.Position.x = Transform.Position.x > Game::MapWidth - PaddingRight ? Game::MapWidth - PaddingRight : Transform.Position.x;
            Transform.Position.y = Transform.Position.y < PaddingTop ? PaddingTop : Transform.Position.y;
            Transform.Position.y = Transform.Position.y > Game::MapHeight - PaddingBottom ? Game::MapHeight - PaddingBottom : Transform.Position.y;
        }   
        
        bool bIsOutsideMap = 
            {
                Transform.Position.x < 0 ||
                Transform.Position.x > Game::MapWidth ||
                Transform.Position.y < 0 ||
                Transform.Position.y > Game::MapHeight
            };
        
        if (bIsOutsideMap && !InEntity.HasTag("Player"))
        {
            InEntity.Kill();
        }
    }
}

void MovementSystem::SubscribeToEvents(const std::unique_ptr<EventManager>& InEventManager)
{
    InEventManager->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
}

void MovementSystem::OnCollision(CollisionEvent& InCollisionEvent)
{
    Entity A = InCollisionEvent.EntityA;
    Entity B = InCollisionEvent.EntityB;

    if (A.BelongsToGroup("Enemies") && B.BelongsToGroup("Obstacles"))
    {
        OnEnemyHitObstalce(A, B);
    }
    
    if (B.BelongsToGroup("Enemies") && A.BelongsToGroup("Obstacles"))
    {
        OnEnemyHitObstalce(B, A);
    }
}

void MovementSystem::OnEnemyHitObstalce(Entity& Enemy, Entity& Obstacle)
{
    if (Enemy.HasComponent<FRigidBodyComponent>() && Enemy.HasComponent<FSpriteComponent>())
    {
        FRigidBodyComponent& EnemyRigidBody = Enemy.GetComponent<FRigidBodyComponent>();
        FSpriteComponent& EnemySprite = Enemy.GetComponent<FSpriteComponent>();
        
        if (std::abs(EnemyRigidBody.Velocity.x) > EPSILON)
        {
            EnemyRigidBody.Velocity.x *= -1.0f;
            EnemySprite.Flip = (EnemySprite.Flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        }
        if (std::abs(EnemyRigidBody.Velocity.y) > EPSILON)
        {
            EnemyRigidBody.Velocity.y *= -1.0f;
            EnemySprite.Flip = (EnemySprite.Flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        }
    }
}


