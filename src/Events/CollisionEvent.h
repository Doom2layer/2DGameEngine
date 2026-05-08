#pragma once
#include "../ECS/ECS.h"
#include "../EventManager/Event.h"

class CollisionEvent : public Event
{
public:
    Entity EntityA;
    Entity EntityB;
    CollisionEvent(Entity InEntityA, Entity InEntityB) : EntityA(InEntityA), EntityB(InEntityB) {}
};
