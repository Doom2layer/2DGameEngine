#include "KeyboardControlSystem.h"

#include "../Components/PlayerControllerComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventManager/EventManager.h"
#include "../Events/KeyPressedEvent.h"

KeyboardControlSystem::KeyboardControlSystem()
{
    RequireComponent<FPlayerControllerComponent>();
    RequireComponent<FSpriteComponent>();
    RequireComponent<FRigidBodyComponent>();
}

void KeyboardControlSystem::SubscribeToEvent(const std::unique_ptr<EventManager>& EventManager)
{
    EventManager->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
}

void KeyboardControlSystem::OnKeyPressed(KeyPressedEvent& Event)
{
    for (Entity Entity : GetSystemEntities())
    {
        const FPlayerControllerComponent PlayerController = Entity.GetComponent<FPlayerControllerComponent>();
        FRigidBodyComponent& RigidBody = Entity.GetComponent<FRigidBodyComponent>();
        FSpriteComponent& Sprite = Entity.GetComponent<FSpriteComponent>();
    
        switch (Event.KeyCode)
        {
        case SDLK_UP:
            RigidBody.Velocity = PlayerController.UpVelocity;
            Sprite.SourceRectangle.y = Sprite.SourceRectangle.h * 0;
            break;
        case SDLK_RIGHT:
            RigidBody.Velocity = PlayerController.RightVelocity;
            Sprite.SourceRectangle.y = Sprite.SourceRectangle.h * 1;
            break;
        case SDLK_DOWN:
            RigidBody.Velocity = PlayerController.DownVelocity;
            Sprite.SourceRectangle.y = Sprite.SourceRectangle.h * 2;
            break;
        case SDLK_LEFT:
            RigidBody.Velocity = PlayerController.LeftVelocity;
            Sprite.SourceRectangle.y = Sprite.SourceRectangle.h * 3;
            break;
        }
    }
}

void KeyboardControlSystem::Update()
{
}
