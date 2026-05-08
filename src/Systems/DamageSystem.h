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
    
};
