#pragma once
#include "../ECS/ECS.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem();
    void SubscribeToEvent(const std::unique_ptr<EventManager>& EventManager);
    void OnKeyPressed(KeyPressedEvent& Event);
    void Update();
};
