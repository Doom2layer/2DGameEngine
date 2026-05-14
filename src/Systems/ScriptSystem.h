#pragma once
#include "../ECS/ECS.h"
#include "sol/sol.hpp"

class ScriptSystem : public System
{
public:
    ScriptSystem();
    void CreateLuaBindings(sol::state& LuaState);
    void Update(double DeltaTime, int FrameDurationMs);
};
