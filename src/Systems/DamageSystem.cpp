#include "DamageSystem.h"

#include "../Components/2DBoxColliderComponent.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"
#include "../ECS/ECS.h"

DamageSystem::DamageSystem()
{
    RequireComponent<F2DBoxColliderComponent>();
}

void DamageSystem::SubscribeToEvents(std::unique_ptr<EventManager>& InEventManager)
{
    InEventManager->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
}

void DamageSystem::OnCollision(CollisionEvent& Event)
{
    Logger::Log("The Damage system received an event collision between entities " + std::to_string(Event.EntityA.GetID()) + " and " + std::to_string(Event.EntityB.GetID()));
    /*
    Event.EntityA.Kill();
    Event.EntityB.Kill();
*/
}
