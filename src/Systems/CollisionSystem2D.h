#pragma once
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Components/TransformComponent.h"
#include "../Components/2DBoxColliderComponent.h"
#include "../Components/2DCircleColliderComponent.h"

class CollisionSystem2D : public System
{
public:
    CollisionSystem2D();
    void Update(std::unique_ptr<EventManager>& EventManagerInstance);
    
    bool CheckAABBCollision(const FTransformComponent& TransformA, const F2DBoxColliderComponent& ColliderA,
    const FTransformComponent& TransformB, const F2DBoxColliderComponent& ColliderB);
    
    /*bool CheckCircleCollision(
        const FTransformComponent& TransformA, const F2DCircleColliderComponent& ColliderA,
        const FTransformComponent& TransformB, const F2DCircleColliderComponent& ColliderB
    );
    
    bool CheckAABBCircleCollision(
        const FTransformComponent& TransformBox, const F2DBoxColliderComponent& BoxCollider,
        const FTransformComponent& TransformCircle, const F2DCircleColliderComponent& CircleCollider
    );*/
};
