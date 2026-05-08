#include "KeyboardControlSystem.h"

#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyPressedEvent.h"

KeyboardControlSystem::KeyboardControlSystem()
{
    RequireComponent<FSpriteComponent>();
    RequireComponent<FRigidBodyComponent>();
}

void KeyboardControlSystem::SubscribeToEvent(std::unique_ptr<EventManager>& EventManager)
{
    EventManager->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
}

void KeyboardControlSystem::OnKeyPressed(KeyPressedEvent& Event)
{
    std::string KeyCode = std::to_string(Event.KeyCode);
    std::string KeySymbol(1, Event.KeyCode);
    Logger::Log("Key pressed: " + KeyCode + " (" + KeySymbol + ")");
}

void KeyboardControlSystem::Update()
{
}
