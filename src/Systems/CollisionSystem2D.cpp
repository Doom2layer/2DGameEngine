#include "CollisionSystem2D.h"

#include <algorithm>

#include "../Components/2DBoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

CollisionSystem2D::CollisionSystem2D()
{
    RequireComponent<FTransformComponent>();
    RequireComponent<F2DBoxColliderComponent>();
}

void CollisionSystem2D::Update()
{
    std::vector<Entity> Entities = GetSystemEntities();

    // Loop all the entities that the system is interested in
    for (std::vector<Entity>::iterator I = Entities.begin(); I != Entities.end(); I++)
    {
        Entity EntityA = *I;
        
        // loop all the entities that still need to be checked to the right of i
        for (std::vector<Entity>::iterator J = I + 1; J != Entities.end(); J++)
        {
            Entity EntityB = *J;
            
            const bool AHasBox = EntityA.HasComponent<F2DBoxColliderComponent>();
            const bool AHasCircle = EntityA.HasComponent<F2DCircleColliderComponent>();
            const bool BHasBox = EntityB.HasComponent<F2DBoxColliderComponent>();
            const bool BHasCircle = EntityB.HasComponent<F2DCircleColliderComponent>();
            
            bool bHasCollided = false;
            
            // A & B have box collision
            if (AHasBox && BHasBox)
            {
                bHasCollided = CheckAABBCollision(
                    EntityA.GetComponent<FTransformComponent>(), 
                    EntityA.GetComponent<F2DBoxColliderComponent>(), 
                    EntityB.GetComponent<FTransformComponent>(), 
                    EntityB.GetComponent<F2DBoxColliderComponent>()
                );
            }
            
            /*
            // A & B have circle collision
            if (AHasCircle && BHasCircle)
            {
                Logger::Log("Checking Circle Collision between Entity " + std::to_string(EntityA.GetID()) + " and Entity " + std::to_string(EntityB.GetID()));
                bHasCollided = CheckCircleCollision(
                    EntityA.GetComponent<FTransformComponent>(), 
                    EntityA.GetComponent<F2DCircleColliderComponent>(), 
                    EntityB.GetComponent<FTransformComponent>(), 
                    EntityB.GetComponent<F2DCircleColliderComponent>()
                );
            }
            
            // A Has a Box and B Has a circle 
            if (AHasBox && BHasCircle)
            {
                Logger::Log("Checking AABB vs Circle Collision between Entity " + std::to_string(EntityA.GetID()) + " and Entity " + std::to_string(EntityB.GetID()));
                bHasCollided = CheckAABBCircleCollision(
                    EntityA.GetComponent<FTransformComponent>(),    
                    EntityA.GetComponent<F2DBoxColliderComponent>(), 
                    EntityB.GetComponent<FTransformComponent>(), 
                    EntityB.GetComponent<F2DCircleColliderComponent>()
                );
            }
            
            // A Has a Circle and B Has a Box
            if (AHasCircle && BHasBox)
            {
                Logger::Log("Checking AABB vs Circle Collision between Entity " + std::to_string(EntityA.GetID()) + " and Entity " + std::to_string(EntityB.GetID()));
                bHasCollided = CheckAABBCircleCollision(
                    EntityB.GetComponent<FTransformComponent>(),    
                    EntityB.GetComponent<F2DBoxColliderComponent>(), 
                    EntityA.GetComponent<FTransformComponent>(), 
                    EntityA.GetComponent<F2DCircleColliderComponent>()
                );
            }
            */
            
            if (bHasCollided)
            {
                Logger::Log("Collision detected between Entity " + std::to_string(EntityA.GetID()) + " and Entity " + std::to_string(EntityB.GetID()));
            }
            
        }
    }
}

// AABB vs AABB — check if rectangles overlap on both axes
bool CollisionSystem2D::CheckAABBCollision(
    const FTransformComponent& TransformA, const F2DBoxColliderComponent& ColliderA,
    const FTransformComponent& TransformB, const F2DBoxColliderComponent& ColliderB)
{
    const float AX = TransformA.Position.x + ColliderA.Offset.x;
    const float AY = TransformA.Position.y + ColliderA.Offset.y;
    const float BX = TransformB.Position.x + ColliderB.Offset.x;
    const float BY = TransformB.Position.y + ColliderB.Offset.y;

    return (
        AX < BX + ColliderB.Width  &&
        AX + ColliderA.Width > BX  &&
        AY < BY + ColliderB.Height &&
        AY + ColliderA.Height > BY
    );
}

/*
// Circle vs Circle — distance between centers less than sum of radii
bool CollisionSystem2D::CheckCircleCollision(const FTransformComponent& TransformA,
    const F2DCircleColliderComponent& ColliderA, const FTransformComponent& TransformB,
    const F2DCircleColliderComponent& ColliderB)
{
    // Center of each circle
    const glm::vec2 CenterA = TransformA.Position + ColliderA.Offset;
    const glm::vec2 CenterB = TransformB.Position + ColliderB.Offset;
    
    // Distance between centers
    const float Distance = glm::distance(CenterA, CenterB);
    
    // Collision if distance is less than sum of radii
    return Distance < ColliderA.Radius + ColliderB.Radius;
}

bool CollisionSystem2D::CheckAABBCircleCollision(const FTransformComponent& TransformBox,
    const F2DBoxColliderComponent& BoxCollider, const FTransformComponent& TransformCircle,
    const F2DCircleColliderComponent& CircleCollider)
{
    // Circle Center
    const glm::vec2 CircleCenter = TransformCircle.Position + CircleCollider.Offset;
    
    // Box Bounds
    const float BoxLeft   = TransformBox.Position.x + BoxCollider.Offset.x;
    const float BoxTop    = TransformBox.Position.y + BoxCollider.Offset.y;
    const float BoxRight  = BoxLeft + BoxCollider.Width;
    const float BoxBottom = BoxTop  + BoxCollider.Height;
    
    // Find the closest point on box to circle center
    const float ClosestX = std::clamp(CircleCenter.x, BoxLeft, BoxRight);
    const float ClosestY = std::clamp(CircleCenter.y, BoxTop, BoxBottom);
    
    // Distance from closest point to circle center
    const float Distance = glm::distance(CircleCenter, glm::vec2(ClosestX, ClosestY));
    
    return Distance < CircleCollider.Radius;  
}
*/

