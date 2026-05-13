#pragma once
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System
{
public:
    MovementSystem();
    void Update(double DeltaTime);
    void SubscribeToEvents(const std::unique_ptr<EventManager>& InEventManager);
    void OnCollision(CollisionEvent& InCollisionEvent);
    void OnEnemyHitObstalce(Entity& Enemy, Entity& Obstacle);
};
