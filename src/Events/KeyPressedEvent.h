#pragma once
#include <SDL_keycode.h>

#include "../EventManager/Event.h"

class KeyPressedEvent : public Event
{
public:
    SDL_Keycode KeyCode;
    explicit KeyPressedEvent(SDL_Keycode InKeyCode) : KeyCode(InKeyCode) {}
};
