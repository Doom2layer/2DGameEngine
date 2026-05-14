#pragma once
#include "../ECS/ECS.h"
#include "sol/sol.hpp"

struct FScriptComponent : public Component<FScriptComponent>
{
    sol::function Function;
    
    FScriptComponent(sol::function InFunction = sol::lua_nil) : Function(InFunction) {}
};
