#pragma once
#include "../ECS/ECS.h"
#include "../Events/KeyPressedEvent.h"

class EventManager;
class ECSManager;

class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem();
    void SubscribeToEvents(std::unique_ptr<EventManager>& InEventManager);
    void OnKeyPressed(KeyPressedEvent& Event);
    void Update(std::unique_ptr<ECSManager>& ECSManagerInstance);
    
};
