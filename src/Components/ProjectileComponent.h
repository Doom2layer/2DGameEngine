#pragma once
#include <SDL_timer.h>

#include "../ECS/ECS.h"

struct FProjectileComponent : public Component<FProjectileComponent>
{
    Uint32 StartTime;
    Uint32 Duration;
    int HitPercentDamage;
    bool bIsFriendly;
    
    FProjectileComponent(
        int ProjectileDuration = 0,
        int ProjectileHitPercentDamage = 0,
        bool bProjectileIsFriendly = false
    )
    : StartTime(SDL_GetTicks())
    , Duration(ProjectileDuration)
    , HitPercentDamage(ProjectileHitPercentDamage)
    , bIsFriendly(bProjectileIsFriendly)
    {}
    
};
