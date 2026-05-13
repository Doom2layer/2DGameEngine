#pragma once

#include <memory>
#include <sol/sol.hpp>
#include "../AssetManager/AssetManager.h"
#include "../ECS/ECS.h"

class LevelLoader
{
public:
    LevelLoader();
    ~LevelLoader();
    
    void LoadLevel(sol::state& LuaState, const std::unique_ptr<ECSManager>& ECSManagerInstance, const std::unique_ptr<AssetManager>& AssetManagerInstance, int LevelNumber);
};
