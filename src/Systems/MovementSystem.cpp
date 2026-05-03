#include "MovementSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"


MovementSystem::MovementSystem()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<FRigidBodyComponent>();
}

void MovementSystem::Update(double DeltaTime)
{
    // Loop all the entities that the system is interested in
    for (const Entity& InEntity : GetSystemEntities())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        const FRigidBodyComponent& RigidBody = InEntity.GetComponent<FRigidBodyComponent>();
        Transform.Position.x += RigidBody.Velocity.x * DeltaTime;
        Transform.Position.y += RigidBody.Velocity.y * DeltaTime;
    }
}


