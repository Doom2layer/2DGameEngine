#pragma once
#include <SDL_timer.h>
#include "../ECS/ECS.h"
#include "glm/vec2.hpp"

struct FProjectileEmitterComponent : public Component<FProjectileEmitterComponent>
{
    glm::vec2 Velocity;
    Uint32    FireRate;          
    Uint32    Duration;          
    int       HitPercentDamage;
    Uint32    LastEmissionTime;
    bool      bIsFriendly;

    explicit FProjectileEmitterComponent(
        glm::vec2 ProjectileVelocity      = glm::vec2(0.0f, 0.0f),
        Uint32    ProjectileFireRate       = 1000,
        Uint32    ProjectileDuration       = 10000,
        int       ProjectileHitPercentDamage = 10,
        bool      bProjectileIsFriendly    = true
    )
    : Velocity(ProjectileVelocity)
    , FireRate(ProjectileFireRate)
    , Duration(ProjectileDuration)
    , HitPercentDamage(ProjectileHitPercentDamage)
    , LastEmissionTime(SDL_GetTicks())
    , bIsFriendly(bProjectileIsFriendly)
    {}
};