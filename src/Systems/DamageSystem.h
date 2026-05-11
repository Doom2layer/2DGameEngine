#pragma once
#include "../ECS/ECS.h"
#include "../Events/CollisionEvent.h"
#include "../EventManager/EventManager.h"

class DamageSystem : public System
{
public:
    DamageSystem();
    
    void SubscribeToEvents(std::unique_ptr<EventManager>& InEventManager);
    
    void OnCollision(CollisionEvent& Event);
    
    void Update();

    void OnProjectileHitPlayer(Entity Projectile, Entity Player);
    void OnProjectileHitEnemey(Entity Projectile, Entity Enemy);
    
};
