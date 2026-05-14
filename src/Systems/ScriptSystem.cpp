#include "ScriptSystem.h"

#include "../Components/AnimationComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

std::tuple<double, double> GetEntityPosition(Entity InEntity)
{
    if (InEntity.HasComponent<FTransformComponent>())
    {
        const FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        return std::make_tuple(Transform.Position.x, Transform.Position.y);
    }
    Logger::Error("GetEntityPosition failed: Entity does not have a TransformComponent.");
    return std::make_tuple(0.0, 0.0);
}

std::tuple<double, double> GetEntityVelocity(Entity InEntity)
{
    if (InEntity.HasComponent<FRigidBodyComponent>())
    {
        const FRigidBodyComponent& RigidBody = InEntity.GetComponent<FRigidBodyComponent>();
        return std::make_tuple(RigidBody.Velocity.x, RigidBody.Velocity.y);
    }
    Logger::Error("GetEntityVelocity failed: Entity does not have a RigidBodyComponent.");
    return std::make_tuple(0.0, 0.0);
    
}

void SetEntityPosition(Entity InEntity, double X, double Y)
{
    if (InEntity.HasComponent<FTransformComponent>())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        Transform.Position.x = X;
        Transform.Position.y = Y;
    }
    else
    {
        Logger::Error("SetEntityPosition failed: Entity does not have a TransformComponent.");
    }
}

void SetEntityVelocity(Entity InEntity, double X, double Y)
{
    if (InEntity.HasComponent<FRigidBodyComponent>())
    {
        FRigidBodyComponent& RigidBody = InEntity.GetComponent<FRigidBodyComponent>();
        RigidBody.Velocity.x = X;
        RigidBody.Velocity.y = Y;
    }
    else
    {
        Logger::Error("SetEntityVelocity failed: Entity does not have a RigidBodyComponent.");
    }
}

void SetEntityRotation(Entity InEntity, double Rotation)
{
    if (InEntity.HasComponent<FTransformComponent>())
    {
        FTransformComponent& Transform = InEntity.GetComponent<FTransformComponent>();
        Transform.Rotation = Rotation;
    }
    else
    {
        Logger::Error("SetEntityRotation failed: Entity does not have a TransformComponent.");
    }
}

void SetEntityAnimationFrame(Entity InEntity, int Frame)
{
    if (InEntity.HasComponent<FAnimationComponent>())
    {
        FAnimationComponent& Animation = InEntity.GetComponent<FAnimationComponent>();
        Animation.CurrentFrame = Frame;
    }
    else
    {
        Logger::Error("SetEntityAnimationFrame failed: Entity does not have an AnimationComponent.");
    }
}

void SetProjectileVelocity(Entity InEntity, double x, double y)
{
    if (InEntity.HasComponent<FProjectileEmitterComponent>())
    {
        FProjectileEmitterComponent& ProjectileEmitter = InEntity.GetComponent<FProjectileEmitterComponent>();
        ProjectileEmitter.Velocity.x = x;
        ProjectileEmitter.Velocity.y = y;
    }
    else
    {
        Logger::Error("SetProjectileVelocity failed: Entity does not have a RigidBodyComponent.");
    }
}

ScriptSystem::ScriptSystem()
{
    RequireComponent<FScriptComponent>();
}

void ScriptSystem::CreateLuaBindings(sol::state& LuaState)
{
    LuaState.new_usertype<Entity>(
        "entity",
        "get_id", &Entity::GetID,
        "destroy", &Entity::Kill,
        "has_tag", &Entity::HasTag,
        "belongs_to_group", &Entity::BelongsToGroup
    );
    LuaState.set_function("get_velocity", GetEntityVelocity);
    LuaState.set_function("get_position", GetEntityPosition);
    LuaState.set_function("set_position", SetEntityPosition);
    LuaState.set_function("set_velocity", SetEntityVelocity);
    LuaState.set_function("set_rotation", SetEntityRotation);
    LuaState.set_function("set_animation_frame", SetEntityAnimationFrame);
    LuaState.set_function("set_projectile_velocity", SetProjectileVelocity);
}

void ScriptSystem::Update(double DeltaTime, int FrameDurationMs)
{
    for (Entity InEntity : GetSystemEntities())
    {
        const FScriptComponent& ScriptComponent = InEntity.GetComponent<FScriptComponent>();

        if (!ScriptComponent.Function)
        {
            continue;
        }
        
        ScriptComponent.Function(InEntity, DeltaTime, FrameDurationMs);
    }
}