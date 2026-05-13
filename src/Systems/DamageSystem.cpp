#include "DamageSystem.h"

#include "../Components/2DBoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../EventManager/EventManager.h"
#include "../Events/CollisionEvent.h"
#include "../ECS/ECS.h"

DamageSystem::DamageSystem()
{
    RequireComponent<F2DBoxColliderComponent>();
}

void DamageSystem::SubscribeToEvents(const std::unique_ptr<EventManager>& InEventManager)
{
    InEventManager->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
}

void DamageSystem::OnCollision(CollisionEvent& Event)
{
    Entity A = Event.EntityA;
    Entity B = Event.EntityB;
    
    if (A.BelongsToGroup("Projectiles") && B.HasTag("player"))
    {
        OnProjectileHitPlayer(A, B); // A is the projectile and B is the player    
    }
    
    if (B.BelongsToGroup("Projectiles") && A.HasTag("player"))
    {
        OnProjectileHitPlayer(B, A); // B is the projectile and A is the player
    }
    
    if (A.BelongsToGroup("Projectiles") && B.BelongsToGroup("enemies"))
    {
        OnProjectileHitEnemey(A, B); // A is the projectile and B is the enemy
    }
    
    if (B.BelongsToGroup("Projectiles") && A.BelongsToGroup("enemies"))
    {
        OnProjectileHitEnemey(B, A); // B is the projectile and A is the enemy
    }
    
}

void DamageSystem::OnProjectileHitPlayer(Entity Projectile, Entity Player)
{
    FProjectileComponent& ProjectileComponent = Projectile.GetComponent<FProjectileComponent>();
    if (!ProjectileComponent.bIsFriendly)
    {
        // Reduce the health of the player by the projectile hitpercentdamage
        FHealthComponent& HealthComponent = Player.GetComponent<FHealthComponent>();
        // Subtract the hit percent damage from the health percentage, ensuring it doesn't go below 0
        HealthComponent.HealthPercentage -= ProjectileComponent.HitPercentDamage;
        
        // Kill the player if health percentage is 0 or below
        if (HealthComponent.HealthPercentage <= 0)
        {
            Player.Kill();
        }
        // Kill the projectile
        Projectile.Kill();
    }
}

void DamageSystem::OnProjectileHitEnemey(Entity Projectile, Entity Enemy)
{
    FProjectileComponent& ProjectileComponent = Projectile.GetComponent<FProjectileComponent>();
    if (ProjectileComponent.bIsFriendly)
    {
        // Reduce the health of the enemy by the projectile hitpercentdamage
        FHealthComponent& HealthComponent = Enemy.GetComponent<FHealthComponent>();
        // Subtract the hit percent damage from the health percentage, ensuring it doesn't go below 0
        HealthComponent.HealthPercentage -= ProjectileComponent.HitPercentDamage;
        
        // Kill the enemy if health percentage is 0 or below
        if (HealthComponent.HealthPercentage <= 0)
        {
            Enemy.Kill();
        }
        // Kill the projectile
        Projectile.Kill();
    }
}
